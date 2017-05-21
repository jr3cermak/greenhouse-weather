/*****************************************************************
  GreenXBee.ino (0.1.1)
  Wireless communication from the greenhouse.

  XBee Setup:
  The XBees need to be setup to communicate with each other by
  setting a common PAN ID.  One of the XBees needs to be setup
  as the "COORDINATOR AT" to allow devices to communicate with each
  other.  The XBees have another mode other than "AT", we chose to
  use "AT" for ease of use.  The communication rate is also set at
  9600.  It does not matter which XBee is in the house vs. the greenhouse.
  If you have many XBee devices, you will want the coordinators to be
  in the middle of the network.

  Setup of XBee Series 2:
  >https://www.hcilab.org/gadgeteer/2012/12/13/xbee-zigbee-howto/
  >https://www.arduino.cc/en/Guide/ArduinoWirelessShieldS2

  Hardware Hookup:
  The XBee Shield makes all of the connections you'll need
  between Arduino and XBee. If you have the shield make
  sure the SWITCH IS IN THE "DLINE" POSITION. That will connect
  the XBee's DOUT and DIN pins to Arduino pins 2 and 3.

  PINS:
  D2 (Arduino's Software RX) -> XBee's DOUT (TX)
  D3 (Arduino's Software TX) -> XBee's DIN (RX)

  Note: For Particle code compatability, we #define digital
  pins with the D prefix.

  Program notes:
  We can turn off Serial console debugging.
  The main communication will be over I2C from a master.
  The main goal is to recieve commands from the master, ask for
  data over XBee and return it to the master.

  I2C Configuration (Greehhouse)
  ADDRESS           DEVICE
  -------           ------
  <master>          Particle Electron
  0x20              Arduino RedBoard/XBee
  0x21              Arduino Mega 2560

  Command Requests (Type: L=Local;R=Remote)
  Command   Type    Description
  -------   ----    -----------
  wd1       R       Weather Data Block # 1:
  gp1       R       GPS Data Block     # 1:
  ncmd      L       Return number of commands in the queue

*****************************************************************/
// Particle code compatibility; add a D prefix to digital pins
const byte D2 = 2;
const byte D3 = 3;

// Define baud rates in which to communicate
const int SERIAL_BAUD = 9600;
const int XBEE_BAUD = 9600;

// Use Arduino standard SoftwareSerial library to communicate with the XBee
#include <SoftwareSerial.h>
// SoftwareSerial(rxPin, txPin, inverse_logic)
SoftwareSerial XBee(D2, D3);

// Use the Arduino standard Wire library to handle I2C communication
#include <Wire.h>
const byte I2C_SLAVE_ADDR = 0x20;

// Define two queues for commands and data passed via I2C
#include <QueueList.h>
// Create string queues
QueueList <String> cmdQ;
QueueList <String> dataQ;

// Define an inbound command buffer
String cmdBuffer = "";

// Debugging options

// 2018-05-19
// All defines enabled: pgm=5044b; glob=501b
// All debugging off  : pgm=4162b; glob=328b

// Enable debugging via the serial interface.  The
// interface is abstracted so we can switch between
// Serial, Serial1, SoftSerial, etc.  See setup()
// REF: http://stackoverflow.com/questions/11865077/arduino-serial-object-data-type-to-create-a-variable-holding-a-reference-to-a-p
#define DEBUG_SERIAL
#if defined(DEBUG_SERIAL)
HardwareSerial *tty;
#endif

// Enable debugging memory size
// 2018-05-19
#define DEBUG_MEMFREE
#if defined(DEBUG_MEMFREE)
#include <MemoryFree.h>
#endif

// Loop debugging
// Slow down processing a bit so the serial console
// isn't swamped with output.  Define DEBUG_LOOP_TICK
// to send a tick to the serial console.
#define DEBUG_LOOP
#if defined(DEBUG_LOOP)
//#define DEBUG_LOOP_TICK
const int DEBUG_LOOP_DELAY = 1000;
#endif

// Functions
// Place functions before setup() and loop() to avoid compiler warnings

// Read from I2C Master via onReceive callback
// Normally, this should be a single command to request data.
// We should always return a byte to the master that is the
// size of the queue or an error code.
void readDataFromMaster(int numBytes) {
  int n = numBytes;
  n++;
  // Read the command from Master
  String inCmd = "";
  while (Wire.available()) {
    char c = Wire.read();
    // Dump CR and LF, this indicates the end of line
    if (c == 10 || c == 13) {
      // If the command is not empty, add it to the queue
      // and clear the variable
      if (inCmd.length() > 0) {
        cmdQ.push(inCmd);
#if defined(DEBUG_SERIAL)
        tty->print(F("qCmd="));
        tty->println(inCmd);
#endif
        inCmd = "";
      }
    } else if (c == 24) {
      // Clear current command in the buffer
      // ASCII 24b = CAN (^X)
      inCmd = "";
    } else {
      inCmd.concat(c);
    }
  }
#if defined(DEBUG_SERIAL)
  tty->print(F("#cmdQ="));
  tty->println(cmdQ.count());
#endif
#if defined(DEBUG_SERIAL) && defined(DEBUG_MEMFREE)
  tty->print(F("mem="));
  tty->println(freeMemory());
#endif
}

// A I2C Master has requested data.
// If there is data to send, send the next message from dataQ,
// otherwise send "EOL\n".
void sendDataToMaster() {
  // Send next dataQ message, otherwise "EOL"
  // This is how to initialize the char array with zero's
  char msg[32] = { 0 };
  String rsp = "EOL\n";
  if (dataQ.count() > 0) {
    rsp = dataQ.pop();
  }

  rsp.toCharArray(msg, rsp.length() + 1);
#if defined(DEBUG_SERIAL)
  tty->print(F("RTS #dataQ="));
  tty->println(dataQ.count());
  //tty->print(F("MSG["));
  //tty->print(msg);
  //tty->println(F("]"));
#endif
  // We will always send 32 bytes
  Wire.write(msg, 32);
}

// This sends messages out the XBee
void sendRemote(String &msgOut) {
  XBee.write(msgOut.c_str());
}

// Command processor
void processCommand() {
  String cmd = "";
  String rsp = "";
  cmd = cmdQ.pop();
  if (cmd == "ncmd") {
    rsp = "#cmdQ:" + String(cmdQ.count());

  } else if (cmd == "mem") {
#if defined(DEBUG_MEMFREE)
    rsp = "#mem:" + String(freeMemory());
#endif    
  } else if (cmd == "wd1") {
    sendRemote(cmd);
  }
  if (rsp.length() > 0) {
    dataQ.push(rsp + "\n");
#if defined(DEBUG_SERIAL)
    tty->print(F("Process cmd:"));
    tty->print(cmd);
    tty->print(F(" Resp:"));
    tty->println(rsp);
#endif
  }
}

// Initialization
void setup() {
  // Initialize communication with the XBee
  XBee.begin(XBEE_BAUD);

  // Initialize I2C communication, setup as a SLAVE
  Wire.begin(I2C_SLAVE_ADDR);
  // Setup callback functions
  // Master has sent a transmission
  Wire.onReceive(readDataFromMaster);
  // Master has requested data
  Wire.onRequest(sendDataToMaster);

#if defined(DEBUG_SERIAL)
  // Change this to the appropriate serial device for debugging
  tty = &Serial;
  // Debugging via serial (compact the code if we don't need it)
  tty->begin(SERIAL_BAUD);
  // Wait for serial port to connect
  while (!tty) {
  }
  tty->println(F("Serial setup() complete."));
#endif
}

// Main program loop
void loop() {
  // Set an activity flag
  // If this is set to true, then we ignore the loop delay
  bool activity = false;
  // Inbound character from serial or XBee
  char c = 0;

  // Process commands
  if (cmdQ.count() > 0) {
    activity = true;
    processCommand();
  }

  // Read from XBee
  if (XBee.available()) {
    c = XBee.read();
  }
#if defined(DEBUG_SERIAL)
  // Read from serial
  if (c == 0 && tty->available()) {
    // Treat serial input as XBee input
    c = tty->read();
  }
#endif

  // Process inbound characters
  if (c != 0) {
    activity = true;
#if defined(DEBUG_SERIAL)
    // If we see a character, send to serial console
    if (c != 24) {
      tty->write(c);
    }
#endif
    switch (c) {
      case 10: // LF  (^J)(\n)
      case 13: // CR  (^M)(\r)
        // If we have something in the buffer, add it to cmdQ
        if (cmdBuffer.length() > 0) {
          cmdQ.push(cmdBuffer);
#if defined(DEBUG_SERIAL)
          tty->print(F("qCmd="));
          tty->println(cmdBuffer);
#endif
          cmdBuffer = "";
        }
        break;
      case 24: // CAN (^X)
        cmdBuffer = "";
        break;
      default:
        cmdBuffer.concat(c);
        break;
    }
  }

#if defined(DEBUG_LOOP)
  // If there was no activity, rest for a short bit
  if (activity == false) {
    delay(DEBUG_LOOP_DELAY);
  }
#endif
#if defined(DEBUG_LOOP) && defined(DEBUG_SERIAL) && defined(DEBUG_LOOP_TICK)
  tty->println(F("loop() tick"));
#endif
}


