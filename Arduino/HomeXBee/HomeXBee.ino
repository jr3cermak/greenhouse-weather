/*****************************************************************
  HomeXBee.ino (0.1.3)
  Wireless communication to the greenhouse from home base
  devices.

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
  D2  (Arduino's Software RX) -> XBee's DOUT (TX)
  D3  (Arduino's Software TX) -> XBee's DIN (RX)
  D12 (RTS to mcp.py, pull LOW to indicate a message is ready) -> BBBW P8_11(GPIO_45)
    This is via a signal level shifter so we don't blow out the BBBW pin

  Note: For Particle code compatability, we #define digital
  pins with the D prefix.

  Program notes:
  We can turn off Serial console when finished debugging.
  The main communication will be over I2C from a master.
  The main goal is to recieve commands from the master, ask for
  data over XBee and return it to the master.

  I2C Configuration
  ADDRESS           DEVICE
  -------           ------
  <master>          BeagleBone Black Wireless
  0x30              Arduino RedBoard/XBee
    RTS/D12         Default(HIGH)/Pull LOW to indicate data ready for transmission to master
  0x31              Particle Photon
  0x32              Particle Electron (FUTURE)

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
const byte D12 = 12;

// Define baud rates in which to communicate
const int SERIAL_BAUD = 9600;
const int XBEE_BAUD = 9600;

// Use Arduino standard SoftwareSerial library to communicate with the XBee
#include <SoftwareSerial.h>
// SoftwareSerial(rxPin, txPin, inverse_logic)
SoftwareSerial XBee(D2, D3);

// Use the Arduino standard Wire library to handle I2C communication
#include <Wire.h>
const byte I2C_SLAVE_ADDR = 0x30;

// Define two queues for commands and data passed via I2C
#include <QueueList.h>
// Create string queues
QueueList <String> cmdQ;
QueueList <String> dataQ;

// Define an inbound command buffer
String msgBuffer = "";

// We are now using the readBytesReg() and write()
// functions of libmraa.   We need to keep track of
// a few things.
// Last register requested
byte lastReg = 0;
// Current message
// Output messages should never be zero in length
String outputMsg = "INIT";
// Input queue select
byte inputQueue = 1;
// Output queue select
byte outputQueue = 2;
// Message sequence
// Default: OLD MESSAGE
byte msgSeq = 1;

// Define register constants
const byte NMSG_INPUT = 1;
const byte NMSG_OUTPUT = 2;
const byte QSEL_INPUT = 3;
const byte QSEL_OUTPUT = 4;
const byte GET_NEXT_MSG = 5;
const byte GET_MSG_SIZE = 6;
const byte FETCH_MSG = 7;
const byte GET_MSG_SEQ = 8;

// Debugging options

// Memory use comparisons
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
const int DEBUG_LOOP_DELAY = 2000;
#endif

// Functions
// Place functions before setup() and loop() to avoid compiler warnings

// We will send data to the master based on
// lastReg sent by readBytesReg().
void sendDataToMaster() {
  switch (lastReg) {
    case NMSG_INPUT:
      switch (inputQueue) {
        case 1: // cmdQ
          Wire.write(cmdQ.count());
          break;
        case 2: // dataQ
          Wire.write(dataQ.count());
          break;
      }    
      break;
    case NMSG_OUTPUT:
      switch (outputQueue) {
        case 1: // cmdQ
          Wire.write(cmdQ.count());
          break;
        case 2: // dataQ
          Wire.write(dataQ.count());
          break;
      }
      break;
    case QSEL_INPUT:
      Wire.write(inputQueue);
      break;
    case QSEL_OUTPUT:
      Wire.write(outputQueue);
      break;
    case GET_NEXT_MSG:
      outputMsg = "";
      switch (outputQueue) {
        case 1: // cmdQ
          if (cmdQ.count() > 0) {
            outputMsg = cmdQ.pop();
          }
          break;
        case 2: // dataQ
          if (dataQ.count() > 0) {
            outputMsg = dataQ.pop();
          }
          break;
      }
      if (outputMsg.length() > 0) {
        Wire.write(1);
      } else {
        Wire.write(2);
      }
      msgSeq = 2;
      break;
    case GET_MSG_SIZE:
      Wire.write(outputMsg.length());
      msgSeq = 3;
      break;
    case FETCH_MSG:
      Wire.write(outputMsg.c_str());
      msgSeq = 1;
      break;
    case GET_MSG_SEQ:
      Wire.write(msgSeq);
      break;
  }

  // If this queue is now empty, return RTS line to HIGH (no data)
  if (dataQ.count() == 0) {
    digitalWrite(D12, HIGH);
  }
}

// Read from I2C Master via onReceive callback
// This will be a one character REGISTER
// If >=2; this will be string input
void readDataFromMaster(int numBytes) {
  // If message is one in length, then we treat this
  // as a register request.
  if (numBytes == 1) {
    lastReg = Wire.read();
#if defined(DEBUG_SERIAL)
    tty->print(F("REG="));
    tty->println(lastReg);
#endif
  } else if (numBytes > 1) {
    // Read inbound message from Master, the actual length is not really
    // important.
    String inMsg = "";
    while (Wire.available()) {
      char c = Wire.read();
      // If we have multiple commands, separate them by ASCII(59)(;)
      if (c == 59) {
        // If the message is not empty, add it to the queue
        // and clear the variable
        if (inMsg.length() > 0) {
          switch (inputQueue) {
            case 1: // cmdQ
              cmdQ.push(inMsg);
              break;
            case 2: // dataQ
              dataQ.push(inMsg);
          }
#if defined(DEBUG_SERIAL)
          tty->print(F("+q="));
          tty->print(inputQueue);
          tty->print(F(":["));
          tty->print(inMsg);
          tty->println(F("]"));
#endif
          inMsg = "";
        }
      } else if (c == 24) {
        // Clear current command in the buffer
        // ASCII 24b = CAN (^X)
        inMsg = "";
      } else {
        inMsg.concat(c);
      }
    }
    // If the message is not empty, add it to the queue
    // and clear the variable
    if (inMsg.length() > 0) {
      switch (inputQueue) {
        case 1: // cmdQ
          cmdQ.push(inMsg);
          break;
        case 2: // dataQ
          dataQ.push(inMsg);
      }
#if defined(DEBUG_SERIAL)
      tty->print(F("$+q="));
      tty->print(inputQueue);
      tty->print(F(":["));
      tty->print(inMsg);
      tty->println(F("]"));
#endif
      inMsg = "";
    }
  }
#if defined(DEBUG_SERIAL)
  tty->print(F("$dataQ="));
  tty->print(dataQ.count());
  tty->print(F(" $cmdQ="));
  tty->println(cmdQ.count());
#endif
#if defined(DEBUG_SERIAL) && defined(DEBUG_MEMFREE)
  tty->print(F("$mem="));
  tty->println(freeMemory());
#endif
}

// This sends messages out the XBee
void sendRemote(String &msgOut) {
  XBee.write(msgOut.c_str());
  XBee.write('\n');
#if defined(DEBUG_SERIAL)
  tty->print(F("XBee>["));
  tty->print(msgOut);
  tty->println("]");
#endif
}

// Command processor: Home
void processCommand() {
  String cmd = "";
  String rsp = "";
  cmd = cmdQ.pop();
  if (cmd == "ncmd") {
    rsp = "$cmdQ:" + String(cmdQ.count());

  } else if (cmd == "mem") {
#if defined(DEBUG_MEMFREE)
    rsp = "$mem:" + String(freeMemory());
#endif
  } else if (cmd == "wd1") {
    sendRemote(cmd);
  }
  if (rsp.length() > 0) {
    dataQ.push(rsp);
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

  // Define a RTS line to mcp.py
  pinMode(D12, OUTPUT);
  // Default signal is HIGH; when a message is ready pull LOW
  digitalWrite(D12, HIGH);

  // This should always be at the end of setup()
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

  // Read from XBee
  if (c == 0 && XBee.available()) {
    c = XBee.read();
  }
#if defined(DEBUG_SERIAL)
  // Read from serial
  if (c == 0 && tty->available()) {
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
        // Parse the message if we see CR and/or LF
        // Messages that start with '$' are data, the rest are commands.
        // Place in appropriate queue
        if (msgBuffer.length() > 0) {
          if (msgBuffer.charAt(0) == '$') {
            dataQ.push(msgBuffer);
#if defined(DEBUG_SERIAL)
            tty->print(F("qData="));
            tty->println(msgBuffer);
#endif
          } else {
            cmdQ.push(msgBuffer);
#if defined(DEBUG_SERIAL)
            tty->print(F("qCmd="));
            tty->println(msgBuffer);
#endif
          }
          msgBuffer = "";
        }
        break;
      case 24: // CAN (^X)
        msgBuffer = "";
        break;
      default:
        msgBuffer.concat(c);
        break;
    }
  }

  // Process commands
  if (activity == false && cmdQ.count() > 0) {
    activity = true;
    processCommand();
  }

  // If we have data in the queue and RTS is HIGH, then lets
  // set it LOW to inform the master to come collect it.
  // 1=HIGH 0=LOW
  if (activity == false && dataQ.count() > 0) {
    int val = 0;
    val = digitalRead(D12);
#if defined(DEBUG_SERIAL)
    tty->print(F("dataQ="));
    tty->print(dataQ.count());
    tty->print(F(" D12State("));
    tty->print(val);
    tty->println(F(")"));
#endif
    if (val == HIGH) {
      digitalWrite(D12, LOW);
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


