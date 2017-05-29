/*
 * Project GreenMcp
 * Description: Master Control Program for the Greenhouse
 * Author: Rob Cermak
 * Date: 2017-05-21
 *
 * Digital PIN Assignments
 * B0       <>
 * B1       <>
 * D0       I2C(SCL)
 * D1       I2C(SDA)
 * D2       RTS signal from GreenXBee
 * D3       Relay 1
 * D4       Relay 2
 * D5       Relay 3
 * D6       Relay 4
 * D7       On board LED
 *
 * I2C Assignments
 * Addr     Device
 * ----     ------
 * <master> GreenMcp.ino
 * 0x20     GreenXBee.ino
 *  RTS     D2 (GreenMcp) <-> LV4(LVL)HV4 <-> D12 (GreenXBee)
 * 0x21     GreenSNet.ino
 *
 * Tasks: T[ODO] -> I[NWORK] -> D[ONE]
 *   T:Cleanup QueueList library; it does some interesting things!
 *   T:Go to deep sleep at <50% SOC; send alerts!
 *   T:Move defines to GreenMcp.h?
 *   T:Act on commands; return data to mcp.py
 *   I:See if we can reduce String use.
 *   I:Refactor communications
 *   D:Don't use "printf" (or anything from C file input/output) in your code!
 *     REF:https://community.particle.io/t/error-compiling-write-r-read-r-etc-library-link-errors/23411/8?u=cermak
 *   D:RTS line is working
 *   D:Begin use of snprintf in I2C communication buffer
 *   D:Add RTS line from GreenMcp to GreenXBee
 *   D:Implement QueueList library; verify it works
 *   D:Replace Serial logging with built-in Logging.
 *   D:Message received from mcp.py
 *   D:Setup simple 1 sec checkStatus timer to poll I2C for incoming commands
 *   D:Use D7 LED to indicate activity (heartbeat)
 *   D:Put device into serial mode to allow "particle flash --serial"
 *
 * Exposed functions:
 *   cmd
 *     serial               Switches device into serial mode
 *     status               Dumps misc information to Log
 *
 * Exposed variables:
 *     loopCt               Basic counter to ensure timer is operating
 */

// Local libraries ../lib
#include <QueueList.h>
#include <RelayShield.h>
#include <Private.h>

// Globals
long loopCt = 0;
int statLed = D7;
int rtsLine = D2;
boolean haveData = false;
int accessCode = 0;
int relayEnabled = 0;
double sysSoc = -1.0;
double sysVolt = -1.0;
String sysMsg = "";

// Defines
#define CCHAR (const char*)
// DEBUGGING
#define DEBUG_LOG
//#define DEBUG_LOG_TICK
//#define DEBUG_SERIAL
//#define DEBUG_SERIAL_MODE_USB
#define DEBUG_TIMER
#define DEBUG_TIMER_DELAY 1000
// PRODUCTION
#define ALLOW_REMOTE_SERIAL_MODE
#define I2CADDR_XBEE 0x20
#define I2CADDR_SNET 0x21
// Define register constants for XBEE
const byte NMSG_INPUT = 1;
const byte NMSG_OUTPUT = 2;
const byte QSEL_INPUT = 3;
const byte QSEL_OUTPUT = 4;
const byte GET_NEXT_MSG = 5;
const byte GET_MSG_SIZE = 6;
const byte FETCH_MSG = 7;
const byte GET_MSG_SEQ = 8;

#if defined(DEBUG_SERIAL)
// Allocate appropriate serial port pointer type
// Use "particle serial monitor" to see traffic
#if defined(DEBUG_SERIAL_MODE_USART)
USARTSerial *tty;
#endif
#if defined(DEBUG_SERIAL_MODE_USB)
USBSerial *tty;
#endif
#endif

// Define two wire pointer
TwoWire *i2c;
// Define a communications buffer that should help against
// memory fragmentation as seen with String use.
char comBuf[32] = { 0 };
// Set pointers to comBuf so we can do some fancy appending
char *comBufPtr = comBuf;
char const *comBufEnd = comBuf + sizeof(comBuf);

// Timers
// One second timer to do some basic checks
#if defined(DEBUG_TIMER)
Timer checkStatus(DEBUG_TIMER_DELAY, everySecond);
#else
Timer checkStatus(1000, everySecond);
#endif
// Timer to automatically turn Relay 1 off when
// it is turned on
Timer ghTimer(30000, turnRelay1Off, true);

// Allocate queues for commands and data
QueueList <String> cmdQ;
QueueList <String> dataQ;
String strBuffer = "";

// Allow us to turn logging completely off in
// production.
#if defined(DEBUG_LOG)
// Enable logging (test)
SerialLogHandler logHandler;
#endif

// Define a Relay object
RelayShield ghRelays;

// FUNCTIONS

// Automatically turn relay #1 off after the
// specified time interval.
void turnRelay1Off() {
  ghRelays.off(1);
  dataQ.push("$Relay1:OFF#");
}

// Parse a new access code passed in the command queue
int parseAccessCode(char nac[]) {
  char newCode[32];
  int newAccCode = 0;
  if (strlen(nac)>4) {
    if (strncmp(nac,"acc:",4) == 0) {
      strcpy(newCode,nac+4);
      newAccCode = atoi(newCode);
#if defined(DEBUG_LOG)
      Log.info("parseAccessCode:%d", newAccCode);
#endif
      if (newAccCode < -9999 || newAccCode > 9999) {
        newAccCode = 0;
      }
    }
  }
  return newAccCode;
}

// Append to current communications buffer
void writeComBuf(char s[]) {
  if (comBufPtr < comBufEnd) {
    comBufPtr += snprintf(comBufPtr, comBufEnd-comBufPtr, "%s", s);
  }
}

// Clear out the contents of the communications buffer and
// reset the pointer
void clearComBuf() {
  memset(comBuf, 0, sizeof(comBuf));
  comBufPtr = comBuf;
}

// Build a readBytesReg(slave, reg_addr, nbytes) function
int readBytesReg(uint8_t slaveAddr, uint8_t i2cReg, int nbytes) {
  char c[2] = { 0 };
  int nret = 0;
  i2c->beginTransmission(slaveAddr);
  i2c->write(i2cReg);
  i2c->endTransmission();
  i2c->requestFrom((uint8_t)slaveAddr,(uint8_t)nbytes);
  clearComBuf();
  while(i2c->available()) {
    c[0] = i2c->read();
    writeComBuf(c);
    nret++;
  }
  if (nret > 0) {
    return nret;
  }

  return 0;
}

// This function reads the RTS line from the
// XBee and returns true or false if there is
// information to exchange.
boolean data_available() {
  int val = digitalRead(rtsLine);
  if (val == 0) {
    return true;
  }
  return false;
}

// Fetch next message out of the currently selected
// output queue.
void fetchData() {
  int ret = 0;
  int seq = 0;
  int sz = 0;
  clearComBuf();
  ret = readBytesReg(I2CADDR_XBEE,GET_MSG_SEQ,1);
#if defined(DEBUG_LOG)
  Log.info("GET_MSG_SEQ RET:%d RES:%d",ret,comBuf[0]);
#endif
  seq = comBuf[0];
  if (seq <= 1) {
    // This indicates an old message, fetch the next one
    clearComBuf();
    ret = readBytesReg(I2CADDR_XBEE,GET_NEXT_MSG,1);
#if defined(DEBUG_LOG)
  Log.info("GET_NEXT_MSG RET:%d RES:%d",ret,comBuf[0]);
#endif
    // If we got a response, read the first char from buffer
    if (ret == 1) {
      ret = comBuf[0];
    }
    switch (ret) {
      case 0:
        // I/O error
        return;
      case 1:
        // Success
        seq = 2;
        break;
      case 2:
        // Failed
        // Consider: increase a rtsErrorCount flag
        break;
    }
  }
  // Fetch # of bytes in the current message
  // This needs to be run if seq == 3 since size is needed
  sz = 0;
  if (seq >= 2) {
    clearComBuf();
    ret = readBytesReg(I2CADDR_XBEE,GET_MSG_SIZE,1);
    // If we got a response, read the first char from buffer
    if (ret == 0) {
      // I/O error
      return;
    }
    sz = comBuf[0];
    if (sz == 0) {
      return;
    }
    // Success
    seq = 3;
#if defined(DEBUG_LOG)
    Log.info("GET_MSG_SIZE RET:%d RES:%d",ret,sz);
#endif
  }
  // Attempt to fetch message
  if (seq == 3 && sz > 0) {
    clearComBuf();
    ret = readBytesReg(I2CADDR_XBEE,FETCH_MSG,sz);
#if defined(DEBUG_LOG)
  Log.info("FETCH_MSG: RET:%d",ret);
#endif
    if (ret == 0) {
      // I/O error
    }
    // Success
    Log.info(">[%s]",comBuf);
    seq = 4;
  }
  // If we succussfully read something (seq == 4),
  // put it in thecorrect queue.
  if (seq == 4) {
    String newMsg = comBuf;
    if (newMsg.length() > 0 && comBuf[0] == '$') {
      // This is a data message -> data queue
      Log.info("+dataQ:[%s]",comBuf);
      dataQ.push(newMsg);
    } else {
      // This is a command message -> command queue
      Log.info("+cmdQ:[%s]",comBuf);
      cmdQ.push(newMsg);
    }
    // Send a ping on a successful read of a message
    // This should trigger the RTS line if all the
    // messages have been read.
    clearComBuf();
    ret = readBytesReg(I2CADDR_XBEE,GET_MSG_SEQ,1);
  }
}

// This function only serves to check on
// the RTS line
void everySecond() {

  // Turn activity light on
  digitalWrite(statLed, HIGH);

  // If there is data available from the GreenXBee,
  // fetch it now.
  if (data_available()) {
    haveData = true;
  } else {
    haveData = false;
  }

  loopCt++;
#if defined(DEBUG_LOG_TICK)
  Log.info("Loop tick: %ld", loopCt);
#endif

  // Turn activity light off
  digitalWrite(statLed, LOW);
}

// Handler for a remote command from the
// Particle Cloud.  Return 1 upon success or
// 0 for no effect.
int remoteCmd(String command) {
#if defined(DEBUG_LOG)
  Log.info("RemoteCommand>%s", CCHAR command);
#endif
  if (command == "serial") {
    if (accessCode != validAccessCode) {
      dataQ.push("$AccessDenied#");
      return -1;
    }
#if defined(ALLOW_REMOTE_SERIAL_MODE)
    // Put device into serial mode
#if (PLATFORM_ID == PLATFORM_ELECTRON_PRODUCTION)
    Cellular.listen();
#endif
#if (PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION)
    WiFi.listen();
#endif
    return 1;
  } else if (command == "status") {
    // Send status info to the logger
#if defined(DEBUG_LOG)
    Log.info("Loop tick: %ld", loopCt);
    Log.info("System version: %s", CCHAR System.version());
#endif
    return 1;
  } else if (
      command == "wd1" || command == "wd2" || command == "wd3"
      || command == "wd4" || command == "wd5" || command == "gp1"
      || command == "gp2" || command == "gp3"
      || command == "r1on" || command == "r1off"
      || command == "r2on" || command == "r2off"
      || command == "r3on" || command == "r3off"
      || command == "r4on" || command == "r4off"
      || command == "relayEnable" || command == "relayDisable"
    ) {
    cmdQ.push(command);
    return 1;
  } else if (strncmp(command,"acc:",4) == 0) {
    cmdQ.push(command);
    return 1;
  }
#endif
  return 0;
}

// This function processes commands on the local system
void runLocalCommand(String cmd) {
  // Allow relay access
  if (relayEnabled == 1) {
    if (cmd == "r1on") {
      ghRelays.on(1);
      ghTimer.start();
      dataQ.push("$Relay1:ON#");
    } else if (cmd == "r1off") {
      ghTimer.stop();
      ghRelays.off(1);
      dataQ.push("$Relay1:OFF#");
    } else if (cmd == "r2on") {
      ghRelays.on(2);
      dataQ.push("$Relay2:ON#");
    } else if (cmd == "r2off") {
      ghRelays.off(2);
      dataQ.push("$Relay2:OFF#");
    }
  }
  if (cmd == "relayEnable") {
    relayEnabled = 1;
      dataQ.push("$Relays:ON#");
  }
  if (cmd == "relayDisable") {
    relayEnabled = 0;
    dataQ.push("$Relays:OFF#");
  }
}

// This function processes queued commands
void processCommand() {
  String cmd = "";
  char rmsg[32];
  cmd = cmdQ.pop();

#if defined(DEBUG_LOG)
  Log.info("Processing command>%s", CCHAR cmd);
#endif
  
  // Update accessCode if we have a command to do so
  if (strncmp(cmd,"acc:",4) == 0) {
    cmd.toCharArray(rmsg,32);
    accessCode = parseAccessCode(rmsg);
    snprintf(rmsg, sizeof(rmsg), "$newAccessCode:%d#", accessCode);
    dataQ.push(rmsg);
  }

  if (accessCode != validAccessCode) {
    dataQ.push("$AccessDenied#");
    return;
  }

  // Commands for GreenSNet
  if (
    cmd == "wd1" || cmd == "wd2" || cmd == "wd3" || cmd == "wd4"
    || cmd == "wd5"
    || cmd == "gp1" || cmd == "gp2" || cmd == "gp3"
  ) {
    getGreenSNet(cmd);
  }
  // Commands that we run locally
  if (
    cmd == "r1on" || cmd == "r1off"
    || cmd == "r2on" || cmd == "r2off"
    || cmd == "relayEnable" || cmd == "relayDisable"
    ) {
    runLocalCommand(cmd);
  }
}

// This routine sends data back to home base.
// There is no return response expected.
void sendData() {
  String msg = "";
  msg = dataQ.pop();
#if defined(DEBUG_LOG)
  Log.info(">GreenXBee:%s", CCHAR msg);
#endif
  // Message size has to be greater than 0
  if (msg.length() > 0) {
    i2c->beginTransmission(I2CADDR_XBEE);
    i2c->write(msg);
    sysMsg = msg;
    i2c->endTransmission();
  }
}

// Processing specific for GreenSNet
// All responses from GreenSNet are placed
// in dataQ.
void getGreenSNet(String cmd) {
  String msg = "";
  i2c->beginTransmission(I2CADDR_SNET);
#if defined(DEBUG_LOG)
    Log.info(">GreenSNet:[%s]", CCHAR cmd);
#endif
  i2c->write(cmd);
  i2c->endTransmission();
  delay(1000);
  //Read number of bytes to expect
  i2c->requestFrom((uint8_t)I2CADDR_SNET,(uint8_t)1);
  byte num = i2c->read();
  i2c->requestFrom((uint8_t)I2CADDR_SNET,(uint8_t)num);
  while(i2c->available()) {
    char c = i2c->read();
    if (c > 0 && c < 255) {
      msg.concat(String(c));
    }
  }
  // If we managed any message, add it
  // to the dataQ.
  if (msg.length() > 0) {
    dataQ.push(msg);
#if defined(DEBUG_LOG)
    Log.info("+dataQ[%d]:[%s]",dataQ.count(), CCHAR msg);
#endif
  }
}

// setup() runs once, when the device is first turned on.
void setup() {
#if defined(DEBUG_LOG)
  Log.info("BEGIN setup()");
#endif

  // Setup pins
  pinMode(statLed, OUTPUT);
  pinMode(rtsLine, INPUT);

  // Set default pin state
  digitalWrite(statLed, LOW);

  // Register Particle functions
  // Allow us to send remote commands
  Particle.function("cmd", remoteCmd);

  // Register Particle variables
  // Allow us to peek at loopCt variable
  Particle.variable("loopCt",loopCt);
  // Disable(0)/enable(1) relay access
  Particle.variable("relayEnabled",relayEnabled);
  // We will now want the valid access code for commands
  // via HomeMcp or Particle cloud.
  Particle.variable("accessCode",accessCode);
  // Show system voltage
  Particle.variable("sysSoc",sysSoc);
  // Show system SOC
  Particle.variable("sysVolt",sysVolt);
  // show system messages
  Particle.variable("sysMsg",sysMsg);

  // Define
#if defined(DEBUG_SERIAL)
  tty = &Serial;
  tty->begin(9600);
#endif

  // Start I2C communication as MASTER
  i2c = &Wire;
  i2c->begin();

  // Start the one second timer
  checkStatus.start();

  // Intialize relays
  ghRelays.begin();
  // make sure they are all off
  ghRelays.allOff();

#if defined(DEBUG_LOG)
  Log.info("END setup()");
#endif
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  boolean activity = false;
  // If we have processing to do, stop the timer, do the work and
  // start again.  Start will restart the timer from the beginning.
  // Process any queued commands
  if (activity == false && cmdQ.count() > 0) {
    checkStatus.stop();
    activity = true;
    processCommand();
  }
  // If there is queued data, send that out.  To keep the loop
  // tight, if we processed a command, skip this until the next
  // go around.
  if (activity == false && dataQ.count() > 0) {
    checkStatus.stop();
    activity = true;
    sendData();
  }
  // If there is no activity, but the slave has work,
  // get it now.
  if (activity == false && haveData) {
    checkStatus.stop();
    activity = true;
    fetchData();
    // We have to check that there is more data
    if (data_available()) {
      haveData = true;
    } else {
      haveData = false;
    }
  }

  // If we had activity, we need to restart the timer
  if (activity == true) {
    checkStatus.start();
  }
}
