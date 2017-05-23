/*****************************************************************
  HomeXBee.ino (0.1.2)
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

// The python library mraa does not keep communication channels open
// for I2C, so read requests must be sequenced through the onRequest
// handler.
int msgseq = 0;

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

// We will send data to the master if RTS is LOW and the master
// sends us byte value of 33.
// If there is data to send, send the next message from dataQ,
// otherwise send "EOL\n".
void sendDataToMaster() {
  // Send next dataQ message, otherwise "EOL"

  static String rsp = "EOL";

#if defined(DEBUG_SERIAL)
#endif

  if (msgseq == 1) {
    if (dataQ.count() > 0) {
      rsp = dataQ.pop();
    } else {
      rsp = "EOL";
    }
    Wire.write(rsp.length());
    msgseq++;
    return;
  }

  if (msgseq == 2) {
    // This is how to initialize the char array with zero's
    char msg[32] = { 0 };
    rsp.toCharArray(msg, rsp.length() + 1);
#if defined(DEBUG_SERIAL)
    tty->print(F("RTS #dataQ="));
    tty->println(dataQ.count());
    tty->print(F("MSG["));
    tty->print(msg);
    tty->println(F("]"));
#endif
    // Send message
    Wire.write(msg, rsp.length() + 1);
    rsp = "EOL";
    msgseq = 0;
  }

  // If this queue is now empty, return RTS line to HIGH (no data)
  if (dataQ.count() == 0) {
    digitalWrite(D12, HIGH);
  }
}

// Read from I2C Master via onReceive callback
// Normally, this should be a single command to request data.
// We should always return a byte to the master that is the
// size of the queue or an error code.
void readDataFromMaster(int numBytes) {
  // Part 1 is one byte long
  // Part 2 is two more more bytes long

  // Part 1 message is always length 1
  if (numBytes == 1) {
    int c = Wire.read();
#if defined(DEBUG_SERIAL)
    tty->print(F("pt1="));
    tty->println(String(c));
#endif
    // Special hook, if c == 33 and RTS is LOW, we
    // actually now send a message back to the master.
    if (c == 33) {
      int val = -1;
      val = digitalRead(D12);
      if (val == 0) {
        msgseq = 1;
      }
    }
    return;
  }

  // Read the command from Master, the actual length is not really
  // important.
  String inCmd = "";
  while (Wire.available()) {
    char c = Wire.read();
    // If we have multiple commands, separate them by ASCII(59)(;)
    if (c == 59) {
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
  // If we have a command, add it to the queue
  if (inCmd.length() > 0) {
    cmdQ.push(inCmd);
#if defined(DEBUG_SERIAL)
    tty->print(F("qCmd="));
    tty->println(inCmd);
#endif
    inCmd = "";
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
  digitalWrite(D12, HIGH);  // Default signal is HIGH; when a message is ready pull LOW

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
        // Parse the message if we see CR and/or LF
        // Messages that start with "$" are data, the rest are commands.
        // Place in appropriate queue
        if (msgBuffer.length() > 0) {
          if (msgBuffer.charAt(0) == "$") {
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


