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
* Verify code for wind sensors
* Order
  * Particle Relay Board

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
