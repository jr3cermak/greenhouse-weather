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
 * Tasks: T[ODO]; D[ONE]; I[NWORK];
 *   T:Cleanup QueueList library; it does some interesting things!
 *   T:Go to deep sleep at <50% SOC; send alerts!
 *   T:Move defines to GreenMcp.h?
 *   T:Act on commands; return data to mcp.py
 *   I:See if we can reduce String use.
 *   I:Refactor communications
 *   D:RTS line is working
 *   D:Begin use of snprintf in I2C communication buffer
 *   D:Add RTS line from GreenMcp to GreenXBee
 *   D:Implement QueueList library; verify it works
 *   D:Replace Serial logging with built-in Logging.
 *   D:Message received from mcp.py
 *   D:Setup simple 1 sec timer to poll I2C for incoming commands
 *   D:Use D7 LED to indicate activity (heartbeat)
 *   D:Put device into serial mode to allow "particle flash --serial"
 *
 * Exposed functions:
 *   cmd
 *     serial               Switches device into serial mode
 *     status               Dumps misc information to Log
 *
 * Exposed variables:
 *     loopct               Basic counter to ensure timer is operating
 */

// Local libraries ../lib
#include <QueueList.h>

// Globals
long loopct = 0;
int statled = D7;
int rtsLine = D2;
double syssoc = -1.0;
double sysvolt = -1.0;

// Defines
#define CCHAR (const char*)
// DEBUGGING
#define DEBUG_LOG
//#define DEBUG_LOG_TICK
//#define DEBUG_SERIAL
//#define DEBUG_SERIAL_MODE_USB
#define DEBUG_TIMER
#define DEBUG_TIMER_DELAY 5000
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
#if defined(DEBUG_TIMER)
Timer timer(DEBUG_TIMER_DELAY, every_second);
#else
Timer timer(1000, every_second);
#endif

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

// FUNCTIONS
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
  i2c->requestFrom(slaveAddr,nbytes);
  clearComBuf();
  while(i2c->available()) {
    c[0] = i2c->read();
    if (c[0] > 0 && c[0] < 255) {
      writeComBuf(c);
      nret++;
    }
  }
  if (nret > 0) {
    return nret;
  }

  return 0;
}

// This function reads the RTS line from the
// XBee and returns true or false if there is
// information to exchange.
bool data_available() {
  int val = digitalRead(rtsLine);
  if (val == 0) {
    return true;
  }
  return false;
}

// Fetch next message out of the currently selected
// output queue.
void fetch_data() {
  int ret = 0;
  int seq = 0;
  int sz = 0;
  clearComBuf();
  ret = readBytesReg(I2CADDR_XBEE,GET_MSG_SEQ,1);
#if defined(DEBUG_LOG)
  Log.info("GET_MSG_SEQ RET: %d RES:%d",ret,comBuf[0]);
#endif
  seq = comBuf[0];
  if (seq == 1) {
    // This indicates an old message, fetch the next one
    clearComBuf();
    ret = readBytesReg(I2CADDR_XBEE,GET_NEXT_MSG,1);
#if defined(DEBUG_LOG)
  Log.info("GET_NEXT_MSG RET:%d RES:%d",ret,comBuf[0]);
#endif
    ret = comBuf[0];
    if (ret == 0) {
      return;
    }
    seq = 2;
  }
  // Fetch # of bytes in the current message
  sz = 0;
  if (seq == 2) {
    clearComBuf();
    ret = readBytesReg(I2CADDR_XBEE,GET_MSG_SIZE,1);
#if defined(DEBUG_LOG)
  Log.info("GET_MSG_SIZE RET:%d RES:%d",ret,comBuf[0]);
#endif
    ret = comBuf[0];
    if (ret == 0) {
      return;
    }
    sz = ret;
    seq = 3;
  }
  // Attempt to fetch message
  if (seq == 3 && sz > 0) {
    clearComBuf();
    ret = readBytesReg(I2CADDR_XBEE,FETCH_MSG,sz);
    Log.info(">%s",comBuf);
  }
}

// This function is run every second
void every_second() {
  bool activity = false;

  // Turn activity light on
  digitalWrite(statled, HIGH);

  // If there is data available from the GreenXBee,
  // fetch it now.
  if (data_available()) {
    fetch_data();
  }

  loopct++;
#if defined(DEBUG_LOG_TICK)
  Log.info("Loop tick: %ld", loopct);
#endif

  // Turn activity light off
  digitalWrite(statled, LOW);
}

// Handler for a remote command from the
// Particle Cloud.  Return 1 upon success or
// 0 for no effect.
int remoteCmd(String command) {
#if defined(DEBUG_LOG)
  Log.info("RemoteCommand=%s", CCHAR command);
#endif
  if (command == "serial") {
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
    Log.info("Loop tick: %ld", loopct);
    Log.info("System version: %s", CCHAR System.version());
#endif
    return 1;
  }
#endif
  return 0;
}

// This routine processes queued commands
void processCommand() {
  String cmd = "";
  cmd = cmdQ.pop();

#if defined(DEBUG_LOG)
  Log.info("Processing command:%s", CCHAR cmd);
#endif

  // Commands for GreenSNet
  if (cmd == "wd1") {
    getGreenSNet(cmd);
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
    msg.concat("\n");
    i2c->write(msg);
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
  delay(2000);
  //Read number of bytes to expect
  i2c->requestFrom(I2CADDR_SNET,1);
  byte num = i2c->read();
  i2c->requestFrom(I2CADDR_SNET,num);
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
  pinMode(statled, OUTPUT);
  pinMode(rtsLine, INPUT);

  // Set default pin state
  digitalWrite(statled, LOW);

  // Register Particle functions
  // Allow us to send remote commands
  Particle.function("cmd", remoteCmd);

  // Register Particle variables
  // Allow us to peek at loopct variable
  Particle.variable("loopct",loopct);

  // Define 
#if defined(DEBUG_SERIAL)
  tty = &Serial;
  tty->begin(9600);
#endif

  // Start I2C communication as MASTER
  i2c = &Wire;
  i2c->begin();

  // Start the one second timer
  timer.start();

#if defined(DEBUG_LOG)
  Log.info("END setup()");
#endif
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  bool activity = false;
  // If we have processing to do, stop the timer, do the work and
  // start again.  Start will restart the timer from the beginning.
  // Process any queued commands
  if (activity == false && cmdQ.count() > 0) {
    timer.stop();
    activity = true;
    processCommand();
  }
  // If there is queued data, send that out.  To keep the loop
  // tight, if we processed a command, skip this until the next
  // go around.
  if (activity == false && dataQ.count() > 0) {
    timer.stop();
    activity = true;
    sendData();
  }

  // If we had activity, we need to restart the timer
  if (activity == true) {
    timer.start();
  }
}
