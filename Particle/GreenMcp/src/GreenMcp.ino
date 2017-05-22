/*
 * Project GreenMcp
 * Description: Master Control Program for the Greenhouse
 * Author: Rob Cermak
 * Date: 2017-05-21
 *
 * I2C Assignments
 * Addr     Device
 * ----     ------
 * <master> GreenMcp.ino
 * 0x20     GreenXBee.ino
 * 0x21     GreenSNet.ino
 *
 * Tasks: T[ODO]; D[ONE]; I[NWORK];
 *   T:Cleanup QueueList library; it does some interesting things!
 *   T:Act on commands; return data to mcp.py
 *   T:Go to deep sleep at <50% SOC; send alerts!
 *   T:Move defines to GreenMcp.h?
 *   I:Implement QueueList library; verify it works
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

// Allocate wire pointer
TwoWire *i2c;

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

// This function is run every second
void every_second() {
  bool activity = false;

  // Turn activity light on
  digitalWrite(statled, HIGH);

  loopct++;
#if defined(DEBUG_LOG_TICK)
  Log.info("Loop tick: %ld", loopct);
#endif
  // Poll GreenXBee over I2C
  // If we have data, do not issue another
  // read request.
  while(i2c->available()) {
    char c = i2c->read();
    switch (c) {
      case 0:
        break;
      case 10:
      case 13:
        if (strBuffer.length() > 0 && strBuffer != "EOL") {
          // If first character is a '$', then we
          // know this is data, otherwise, it is
          // a command.
          if (strBuffer.charAt(0) == '$') {
            dataQ.push(strBuffer);
#if defined(DEBUG_LOG)
            Log.info("+dataQ[%d]:[%s]",dataQ.count(), CCHAR strBuffer);
#endif
          } else {
            cmdQ.push(strBuffer);
#if defined(DEBUG_LOG)
            Log.info("+cmdQ[%d]:[%s]",cmdQ.count(), CCHAR strBuffer);
#endif
          }
          activity = true;
        }
        // Clear out the string buffer
        strBuffer = "";
        break;
      case 24: // CAN(^X): Clear the buffer
        strBuffer = "";
        activity = true;
        break;
      default:
        activity = true;
        strBuffer.concat(c);
        break;
    }
  }
  // If we don't see any activity, make
  // a request, go do stuff and read
  // again in a bit.
  if (activity == false) {
    i2c->requestFrom(I2CADDR_XBEE,32);
  }

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
    Log.info("Loop tick: %d", loopct);
    Log.info("System version: %s", CCHAR System.version());
#endif
    return 1;
  }
#endif
  return 0;
}

// setup() runs once, when the device is first turned on.
void setup() {
#if defined(DEBUG_LOG)
  Log.info("BEGIN setup()");
#endif

  // Setup pins
  pinMode(statled, OUTPUT);

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

// This routine sends data back to home base
void sendData() {
  String msg = "";
  msg = dataQ.pop();

#if defined(DEBUG_LOG)
  Log.info("Returning data:%s", CCHAR msg);
#endif
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  bool activity = false;
  // If we have processing to do, stop the timer, do the work and
  // start again.  Start will restart the timer from the beginning.
  // Process any queued commands
  if (cmdQ.count() > 0) {
    activity = true;
    processCommand();
    timer.stop();
  }
  // If there is queued data, send that out.  To keep the loop
  // tight, if we processed a command, skip this until the next
  // go around.
  if (activity == false && dataQ.count() > 0) {
    activity = true;
    sendData();
    timer.stop();
  }

  // If we had activity, we need to restart the timer
  if (activity == true) {
    timer.start();
  }
}
