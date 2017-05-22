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
 *   I:Implement QueueList library; verify it works
 *   D:Message received from mcp.py
 *   D:Setup simple 1 sec timer to poll I2C for incoming commands
 *   D:Use D7 LED to indicate activity (heartbeat)
 *   D:Put device into serial mode to allow "particle flash --serial"
 */

// Local libraries ../lib
#include <QueueList.h>

// Globals
int statled = D7;
double syssoc = -1.0;
double sysvolt = -1.0;

// Defines
#define DEBUG_SERIAL
#define DEBUG_SERIAL_MODE_USB
#define REMOTE_SERIAL_MODE
#define DEBUG_TIMER
#define DEBUG_TIMER_DELAY 5000
#define I2CADDR_XBEE 0x20
#define I2CADDR_SNET 0x21

#if defined(DEBUG_SERIAL)
// Allocate serial port pointer
#if defined(DEBUG_SERIAL_MODE_USART)
USARTSerial *tty;
#endif
#if defined(DEBUG_SERIAL_MODE_USB)
// Use "particle serial monitor" to see traffic
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

// FUNCTIONS

// This function is run every second
void every_second() {
  bool activity = false;
  static int count = 0;

  // Turn activity light on
  digitalWrite(statled, HIGH);

  count++;
#if defined(DEBUG_SERIAL)
  tty->println(count);
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
#if defined(DEBUG_SERIAL)
            tty->print("dataQ:");
#endif
            dataQ.push(strBuffer);
          } else {
#if defined(DEBUG_SERIAL)
            tty->print("cmdQ:");
#endif
            cmdQ.push(strBuffer);
          }
#if defined(DEBUG_SERIAL)
          tty->println(strBuffer);
#endif
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

int remoteCmd(String command) {
#if defined(DEBUG_SERIAL)
  tty->print("RemoteCommand=");
  tty->println(command);
#endif
#if defined(REMOTE_SERIAL_MODE)
  if (command=="serial") {
    // Put device into serial mode
#if (PLATFORM_ID == PLATFORM_ELECTRON_PRODUCTION)
    Cellular.listen();
#endif
#if (PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION)
    WiFi.listen();
#endif
    return -1;
  }
#endif
  return 0;
}

// setup() runs once, when the device is first turned on.
void setup() {
  // Setup pins
  pinMode(statled, OUTPUT);

  // Set default pin state
  digitalWrite(statled, LOW);

  // Pointer assignments
  tty = &Serial;
  i2c = &Wire;

  // Allow us to send remote commands
  Particle.function("cmd", remoteCmd);
#if defined(DEBUG_SERIAL)
  tty->begin(9600);
#endif
  // Start I2C communication as MASTER
  i2c->begin();

  // Start the one second timer
  timer.start();
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // Process any queued commands
  if (cmdQ.count() > 0) {

  }
  // If there is queued data, send that out
  if (dataQ.count() > 0) {
  }
}
