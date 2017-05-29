/*****************************************************************
  GreenXBee.ino (0.1.3)
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
  We can turn off Serial when finished debugging.
  The main communication will be over I2C from a master.
  The main goal is to recieve commands from the master, ask for
  data over XBee and return it to the master.

  I2C Configuration (Greehhouse)
  ADDRESS           DEVICE
  -------           ------
  <master>          Particle Electron
  0x20              Arduino RedBoard/XBee
    RTS             D12 (GreenXBee) -> HV4(LVL)LV4 -> D2 (GreenMcp)
  0x21              Arduino Mega 2560

  Command Requests (Type: L=Local;R=Remote)
  Command   Type    Description
  -------   ----    -----------
  wd1       R       Weather Data Block # 1: temperature, humidity, temperature
  gp1       R       GPS Data Block     # 1: latitude, longitude
  gp2       R       GPS Data Block     # 2: altitude, number of satellites
  gp3       R       GPS Data Block     # 3: date, time
  ncmd      L       Return number of commands in the queue
  mem       L       Report free memory using MemoryFree library

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
const byte I2C_SLAVE_ADDR = 0x20;

// Define two queues for commands and data passed via I2C
#include <QueueList.h>
// Create string queues
QueueList <String> cmdQ;
QueueList <String> dataQ;

// Define an inbound command buffer
String msgBuffer = "";

// Current message
// Output messages should never be zero in length
String outputMsg = "INIT";
// Queue types: 1 = Command; 2 = Data
const byte COMMAND_QUEUE = 1;
const byte DATA_QUEUE = 2;
// Input queue select (I2C)
byte inputQueue = COMMAND_QUEUE;
// Output queue select (I2C)
byte outputQueue = COMMAND_QUEUE;
// Message sequence (last request)
// 0 : I/O error
// 1 : Next message was requested
// 2 : Get message size
// 3 : Get message
const byte MSG_SEQ_IO = 0;
const byte MSG_SEQ_NEXTMSG = 1;
const byte MSG_SEQ_SIZE = 2;
const byte MSG_SEQ_GETMSG = 3;
byte msgSeq = MSG_SEQ_IO;

// Define register constants
const byte NMSG_INPUT = 1;
const byte NMSG_OUTPUT = 2;
const byte QSEL_INPUT = 3;
const byte QSEL_OUTPUT = 4;
const byte GET_NEXT_MSG = 5;
const byte GET_MSG_SIZE = 6;
const byte FETCH_MSG = 7;
const byte GET_MSG_SEQ = 8;
// We are now using the readBytesReg() and write()
// functions of libmraa.   We need to keep track of
// a few things.
// Last register requested
byte lastReg = 0;

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
const int DEBUG_LOOP_DELAY = 1000;
#endif

// Functions
// Place functions before setup() and loop() to avoid compiler warnings

// We will send data to the master based on
// lastReg sent by readBytesReg().
void sendDataToMaster() {
#if defined(DEBUG_SERIAL)
  tty->print(F("msgSeq>"));
  tty->print(msgSeq);
  tty->print(F(" lastReg>"));
  tty->println(lastReg);
#endif
  switch (lastReg) {
    case NMSG_INPUT:
      switch (inputQueue) {
        case COMMAND_QUEUE:
          Wire.write(cmdQ.count());
          break;
        case DATA_QUEUE:
          Wire.write(dataQ.count());
          break;
      }
      break;
    case NMSG_OUTPUT:
      switch (outputQueue) {
        case COMMAND_QUEUE:
          Wire.write(cmdQ.count());
          break;
        case DATA_QUEUE:
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
      msgSeq = MSG_SEQ_NEXTMSG;
      outputMsg = "";
      switch (outputQueue) {
        case COMMAND_QUEUE:
          if (cmdQ.count() > 0) {
            outputMsg = cmdQ.pop();
          }
          break;
        case DATA_QUEUE:
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
      break;
    case GET_MSG_SIZE:
      Wire.write(outputMsg.length());
      msgSeq = MSG_SEQ_SIZE;
      break;
    case FETCH_MSG:
      Wire.write(outputMsg.c_str());
      msgSeq = MSG_SEQ_GETMSG;
      break;
    case GET_MSG_SEQ:
      // Master will continue to poll this register until
      // messages queue is empty.  We will let it poll one
      // extra time to ensure we get past any I/O errors.
      Wire.write(msgSeq);
      // Special case, if we see this after a FETCH_MSG,
      // then set the msgSeq back to MSG_SEQ_IO.
      if (msgSeq == 3) {
        msgSeq = MSG_SEQ_IO;
      }
      switch (outputQueue) {
        case COMMAND_QUEUE:
          if (cmdQ.count() == 0) {
            digitalWrite(D12, HIGH);
          }
          break;
        case DATA_QUEUE:
          if (dataQ.count() == 0) {
            digitalWrite(D12, HIGH);
          }
          break;
      }
      break;
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
  } else if (numBytes > 1) {
    // Read inbound message from Master, the actual length is not really
    // important.
    String inMsg = "";
    // The default input queue is determined by the inputQueue, but
    // if the message begins with '$' we enforce this to be put into
    // the data queue.
    int defaultQueue = inputQueue;
    while (Wire.available()) {
      char c = Wire.read();
      // If we have multiple commands, separate them by ASCII(59)(;)
      if (c == 59) {
        // If the message is not empty, add it to the queue
        // and clear the variable
        if (inMsg.length() > 0) {
          defaultQueue = inputQueue;
          if (inMsg.charAt(0) == '$') {
            defaultQueue = DATA_QUEUE;
          }
          switch (defaultQueue) {
            case COMMAND_QUEUE:
              cmdQ.push(inMsg);
              break;
            case DATA_QUEUE:
              dataQ.push(inMsg);
          }
#if defined(DEBUG_SERIAL)
          tty->print(F("+q="));
          tty->print(defaultQueue);
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
      defaultQueue = inputQueue;
      if (inMsg.charAt(0) == '$') {
        defaultQueue = DATA_QUEUE;
      }
      switch (defaultQueue) {
        case COMMAND_QUEUE:
          cmdQ.push(inMsg);
          break;
        case DATA_QUEUE:
          dataQ.push(inMsg);
      }
#if defined(DEBUG_SERIAL)
      tty->print(F("$+q="));
      tty->print(defaultQueue);
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
  tty->print(F("XBee>"));
  tty->println(msgOut);
#endif
}

// Command processor: Greenhouse
// XBee Serial -> cmdQ
// cmdQ -> I2C
// I2C:
//   All return data -> XBee Serial
void processCommand() {
  String cmd = "";
  String rsp = "";
  cmd = cmdQ.pop();
#if defined(DEBUG_SERIAL)
  tty->print(F("Process cmd:"));
  tty->println(cmd);
#endif
  if (cmd == "ncmd") {
    rsp = "$cmdQ:" + String(cmdQ.count());
  } else if (cmd == "mem") {
#if defined(DEBUG_MEMFREE)
    rsp = "$mem:" + String(freeMemory());
#endif
  } else if (
    cmd == "wd1" || cmd == "wd2" || cmd == "wd3" || cmd == "wd4" || cmd == "wd5"
    || cmd == "gp1" || cmd == "gp2" || cmd == "gp3"
    || cmd == "r1on" || cmd == "r1off" || cmd == "r2on" || cmd == "r2off"
    || cmd == "r3on" || cmd == "r3off" || cmd == "r4on" || cmd == "r4off"
    || cmd.startsWith("acc:")
    || cmd == "relayEnable" || cmd == "relayDisable"
  ) {
    rsp = cmd;
  }
  if (rsp.length() > 0) {
    dataQ.push(rsp);
#if defined(DEBUG_SERIAL)
    tty->print(F("Process Response:"));
    tty->print(rsp);
    tty->print(F(" #dataQ="));
    tty->println(dataQ.count());
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
  tty->println(F("GreenXBee: Serial setup() complete."));
#endif
}

// Main program loop
void loop() {
  // Set an activity flag
  // If this is set to true, then we ignore the loop delay
  bool activity;
  // Inbound character from serial or XBee
  char c;

  // Top of loop make sure things are intialized
  activity = false;
  c = 0;

  // Process commands
  // We don't process commands here
  //if (cmdQ.count() > 0) {
  //  activity = true;
  //  processCommand();
  //}

  // Read from XBee
  if (c == 0 && XBee.available()) {
    c = XBee.read();
  }
#if defined(DEBUG_SERIAL)
  // Read from serial and we didn't read anything from the XBee
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
      //tty->write(c);
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
            tty->print(F("+qData="));
            tty->println(msgBuffer);
#endif
          } else {
            cmdQ.push(msgBuffer);
#if defined(DEBUG_SERIAL)
            tty->print(F("+qCmd="));
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

  // If we don't have any activity, check out dataQ
  // If there is data, send a line back.
  if (activity == false && dataQ.count() > 0) {
    String dataMsg = "";
    activity = true;
    dataMsg = dataQ.pop();
    sendRemote(dataMsg);
  }

  // If there are commands to send, set the RTS LOW to
  // tell the master to come collect it.
  if (activity == false && cmdQ.count() > 0) {
    int val = digitalRead(D12);

#if defined(DEBUG_SERIAL)
    tty->print(F("cmdQ="));
    tty->print(cmdQ.count());
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


