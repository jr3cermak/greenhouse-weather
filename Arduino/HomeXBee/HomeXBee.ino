/*****************************************************************
  HomeXBee.ino
  This is our wireless communication to the greenhouse.

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

  I2C Configuration
  ADDRESS           DEVICE
  -------           ------
  <master>          BeagleBone Black Wireless
  0x30              Arduino RedBoard/XBee
  0x31              Particle Photon
  0x32              Particle Electron (FUTURE)
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
const byte I2C_SLAVE_ADDR = 0x30;

// Define two queues for commands and data passed via I2C
#include <QueueList.h>
// Create string queues
QueueList <String> cmdQ;
QueueList <String> dataQ;

// Debugging options

// Enable debugging via the serial interface
// 1=true; 0=false
// 2018-05-19
// Serial defined: pgm=5044b; glob=501b
// Serial undef  : pgm=4162b; glob=328b
#define DEBUG_SERIAL

// Enable debugging memory size
// 2018-05-19
#define DEBUG_MEMFREE
#if defined(DEBUG_MEMFREE)
#include <MemoryFree.h>
#endif

// Loop debugging
// Useful to slow the loop down so the serial output
// isn't swamped with output
#define DEBUG_LOOP
#if defined(DEBUG_LOOP)
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
        Serial.print(F("qCmd="));
        Serial.println(inCmd);
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
  Serial.print(F("#cmdQ="));
  Serial.println(cmdQ.count());
#endif
#if defined(DEBUG_SERIAL) && defined(DEBUG_MEMFREE)
  Serial.print(F("mem="));
  Serial.println(freeMemory());
#endif
}

// A I2C Master has requested data.
// If there is no data in the queue (dataQ), return
// an error code (not ready to send; -1).
void sendDataToMaster() {
#if defined(DEBUG_SERIAL)
  Serial.print(F("RTS #dataQ="));
  Serial.println(dataQ.count());
#endif
  // If there is nothing in the queue, return not ready to send = -1 (unsigned 255)
  if (dataQ.count() == 0) {
    Wire.write(-1);
  } else {
    char msg[32];
    String rsp = "";
    rsp = dataQ.pop();
    rsp.toCharArray(msg,rsp.length()+1);
    Wire.write(msg,rsp.length());
  }
}

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
  // Debugging via serial (compact the code if we don't need it)
  Serial.begin(SERIAL_BAUD);
  // Wait for serial port to connect
  while (!Serial) {
  }
  Serial.println(F("Serial setup() complete."));
#endif
}

// Command processor
void processCommand() {
  String cmd = "";
  String rsp = "";
  cmd = cmdQ.pop();
  if (cmd == "ncmd") {
    rsp = "#cmdQ:" + String(cmdQ.count());
  }
  if (rsp.length() > 0) {
    dataQ.push(rsp + "\n");
#if defined(DEBUG_SERIAL)
    Serial.print(F("Process cmd:"));
    Serial.print(cmd);
    Serial.print(F(" Resp:"));
    Serial.println(rsp);
#endif      
  }
}

// Perpetual loop
void loop() {
  // Process commands
  if (cmdQ.count() > 0) {
    processCommand();
  }

#if defined(DEBUG_SERIAL)
  // Debugging via serial
  if (Serial.available())
  { // If data comes in from serial monitor, send it out to XBee
    XBee.write(Serial.read());
  }
  if (XBee.available())
  { // If data comes in from XBee, send it out to serial monitor
    Serial.write(XBee.read());
  }
#endif

#if defined(DEBUG_LOOP)
  delay(DEBUG_LOOP_DELAY);
#endif
#if defined(DEBUG_LOOP) && defined(DEBUG_SERIAL)
  //Serial.println(F("loop() tick"));
#endif
}


