# 2017-05-20
Our first command from HomeXbee.ino made it to the greenhouse XBee.  Now
to get GreenXbee.ino started to respond.

Refactored HomeXbee.ino so the serial device can be easily changed without
changing a lot of code.  The python mraa library is a pain.  We are just
going to pass 32 bytes.  We can't read char by char as the readByte() 
function will just read a byte and trash the rest of the message.  We
might consider refactoring back to the python module smbus.   There is
a smbus2 library.  Will clean up the python code and post that soon.

Time to look at GreenXbee.ino code.  The goal to recieve a request and
ask for data via I2C and pass back to the HomeXBee.ino code.

# 2017-05-19
We are going with a polling situation where the BBBW in the house will poll for
data and status in the greenhouse.  We can either get to it via I2C/XBee or
via TCP.  We might want to add in an Electron at the house as fallback to 
provide Cell <-> Cell emergency backup.

The Electrons could be setup as overall watchdogs with a relay setup as a 
main power reset?  We would have to have the Electrons boot first and then
turn on a relay to turn on main power for the rest of the electronics.  This
could be a mechanism to go completely to sleep if power in these locations
goes south.

Time to begin main programming!  

House side:
* Particle Photon: HomeNet.ino
* Arduino SparkFun RedBoard: HomeXBee.ino
* BeagleBone Black Wireless: mcp.py

Greenhouse side:
* Particle Electron: GreenMcp.ino
* Arduino Mega 2560: GreenSNet.ino
* Arduino SparkFun RedBoard: GreenXbee.ino

TODO:
* Hookup Taylor wind sensor
* Need voltage divider circuit to sense 12V battery (main power)
* Verify code for wind sensors
* Order
  * Particle Relay Board

Use Arduino QueueList library to extend ease of I2C communication by
queuing commands and responses.

A basic program for HomeXBee.ino is working.  A python program based on the
library mraa to talk to the Arduino over I2C works.  It isn't the best as
it lacks a lot of the Arduino features.  We are going to rely on reading
bytes from the Arduino.  Need to filter responses from missing data (char=0),
no data available(char=255) and the good data.

# 2017-05-18
Whoa that was painful.  Getting a Beagle Bone Black Wireless I2C to talk to
the Arduino Mega 2560.  The python mraa library works, but does not have 
anywhere near the capabilities of the Wire library.   For now, we might be
stuck with the BBBW as the master.  

Time to diagram the communication and devices as we know things can talk to
one another over I2C.   What is going to be the least painful?

# 2017-05-17
TODO:
1. Assemble the irrigation system
2. Integrate the 50W solar panel
3. Establish I2C communication between a Photon, RedBoard/XBee and a Beagle Bone Black Wireless or Raspberry Pi 3 (or both!)
4. Establish I2C communication between the Mega 2560 and the RedBoard/XBee
5. Begin coding
6. Harden wiring from breadboards to proto boards

Struggling with the Beagle Bone Black computers.  I updated the flash on two BBB wireless boards.  One
seems to work ok and the other, the wireless does not initialize.  I am about to throw this lot into
the junk heap and work with a Raspberry Pi 3.  I will reflash the BBBWs one more time and being careful.

It also seems like the BBBW want to act as access points.  I really want to be able to reboot them on
demand and have them connect to the established wireless connection at home.  The one that is working
is only sporadically working to allow ssh connections over the wifi.   Very disappointing.
# 2017-05-16
Tested YL-69 sensor.  Added a small bit of code.
The SHT10 sensor is also tested and ready to go.
# 2017-05-15
Create repository for collecting project information and to share to a wider audience.
