# 2017-06-18

Porting axtls library as a possible replacement to the TlsTcpClient.
So far, the firmware size is about 40% smaller than the mbed openssl
stack.  It is far from working.  I am using the TlsTcpClient as
a template.   So far so good.  We are at the point where we are 
comparing this port output against another in debug mode on a Linux
platform.   There are a couple borrowed byteswapping routines that
may or may not be correct for the Particle platform.

REF:

* http://axtls.sourceforge.net/
* https://developer.mbed.org/users/vshymanskyy/code/Blynk/docs/b942afadf9be/BlynkProtocolDefs_8h_source.html
* https://fossies.org/linux/misc/mosh-1.3.0.tar.gz/mosh-1.3.0/src/crypto/byteorder.h?m=t

```
attempting to compile firmware 
downloading binary from: /v1/binaries/5945f1d65a446b1d25f08ccf
saving to: tmp.bin
Memory use: 
  text    data     bss     dec     hex filename
 47892      184    1816   49892    c2e4 /workspace/target/workspace.elf

Compile succeeded.
Saved firmware to: /Users/cermak/Particle/projects/axtls/src/tmp.bin

0000010133 [comm.sparkprotocol] INFO: Received TIME response: 1497756140
0000011099 [comm.sparkprotocol] INFO: Sending A describe message
0000011243 [comm.sparkprotocol] INFO: Sending S describe message
0000020245 [axtls] INFO: Trying to connect
0000020245 [axtls] INFO: begin connect()
0000020246 [axtls] INFO: host:jupyter.lccllc.info port:4443
0000020251 [axtls] INFO: init() ssl_ctx(0x20007658) send_Tls(0x80a2b8d) recv_Tls(0x80a2b31)
0000020252 [axtls] INFO: Assigning i/o pathways ssl_ctx(0x20007658) f_send(0x80a2b8d) f_recv(0x80a2b31)
0000020252 [axtls] INFO: connect() try 1
0000020483 [axtls] INFO: ssl_client_new()
0000020484 [axtls] INFO: ssl->version:0x33
0000020484 [axtls] INFO: do_client_connect()
0000020484 [axtls] INFO: send_client_hello() ssl(0x20008b10)
0000020484 [axtls] INFO: send_packet()
0000020485 [axtls] INFO: send_raw_packet() length=67 msg_length=67
0000020485 [axtls] INFO: SOCKET_WRITE ssl(0x20008b10) t:72 s:0
0000020485 [axtls] INFO: sendParticle ssl(0x20008b10) ssl_ctx(0x20007658) f_send(0x80a2b8d)
0000020486 [axtls] INFO: Wanted to send 72 bytes, sent 72 bytes
0000020486 [axtls] INFO: Handshake error: -1

```

# 2017-06-08

Reworked two SparkFun sensor libraries so they do not re-initialize
as I2C masters.  This code was moved into the Electron without an
issue.   Still have some packet drops.   Discovered where an I2C
clock line got disconnected.  This caused a loopback condition.
Will rewrite responses in JSON before dealing with dropped packets.
The Ubidots API website throws a lot of errors.

# 2017-05-29

Now for general code and wiring cleanup (hardening).  Need to calibrate
and deploy some additional sensors.

Communication now working with a minimal setup to allow the relay to turn on
for 30 seconds and turn off.  One casualty so far, one 5A fuse.

This shows some sample commands send from inside to the greenhouse and
data returning.  Weather data off the SparkFun Weather Shield (wd1/gp3),
on board temperature off the pressure sensor and humidity/temperature
sensor.  GPS data time/date from the GPS module.  Soil temperature/soil
humidity off a SHT10 sensor (wd5).

```
gp3
sent> gp3 len( 3 )
< $DT=05/29/2017,TM=19:08:12#
wd1
sent> wd1 len( 3 )
< $TP=89.4,HH=15.68,HT=89.4#
wd5
sent> wd5 len( 3 )
< $ST=61.36,SH=97.70#
```
We may want to consider rewriting these responses in json so they are
immediately compatible with Node-Red, Ubidots and Particle.

The XBee communication is not perfect.  There are drop outs.  Not totally
unexpected since I am transmitting through several walls.  I just reasoned
out a way to check on the last packet sent.  New command: lps (last packet
sent).

Adding the RTS line from the Arduino back to the Electron and BBBW works
quite well.  We have to trap for conditions where the Arduino loses power
dropping the RTS line.  There are attempts to get data, but the device
is not there.

Added a bit of security.  There is a four digit access code that you can
set in Private.h.  Note: You will not find it on the github site.  You
need to write it.  It will contain all the details not to be shared
publicly.  If you create a new repository, be sure to include it in your
.gitignore file.

# 2017-05-24

Data communications somewhat restored in the refactoring.  The second
RTS line is working.  
Command goes from mcp.py -> HomeXBee -> GreenXBee -> GreenMcp
```
0000035166 [app] INFO: GET_MSG_SEQ RET: 1 RES:1
0000035214 [app] INFO: GET_NEXT_MSG RET:1 RES:1
0000035272 [app] INFO: GET_MSG_SIZE RET:1 RES:3
0000035330 [app] INFO: >wd1
```

General:
* We need to guard against zero length messages in the queues

mcp.py:
* Fixed up some spelling mistakes in variables

GreenXbee.ino:
* Data coming in is queued by looking for '$' at start of the message.
* Use new RTS line to indicate when commands are ready for the
GreenMcp(MASTER).

GreenMcp.ino:
* Setup RTS pin on D2
* Refactor communication and utilize new RTS line from GreenXBee
* Has a similar function to python mraa library: readBytesReg()
  that uses a communications buffer for messages.  Uses snprintf
  to reduce memory fragmentation.

FUTURE CONSIDERATIONS:
* https://community.particle.io/t/safe-way-to-build-strings-without-memory-fragmentation/27150/5
* We may need a daily or periodic reboot of devices, at least the
Photon/Electron(s) to clean up any memory fragmentation due to 
String use.   We will not implement this now to see how long the
devices run without reboot.
* The QueueList might be adaptable later to use a
const char* block with a linked list of structures that address
strings in the block.  This may be a specialized queue for Strings
and break the templating nature of the QueueList library.

TODO:
* Refactor GreenMcp.ino
* Install RTS line between GreenMcp and GreenXBee to alleviate the
unecessary polling between these two devices.
* Re-test communication lines.
* Add i2cTest.ino and i2cTest.py code for testing and demonstration
of how the I2C is used in this project.

Refactoring using I2C register types.  This is very efficient for
sending appropriate signals and returning key information for data
and command queues.  The message passing is now also incorporated
into that scheme and seems to operate much better using the mraa
library.

# 2017-05-23

Success.  The HomeXBee and mcp.py can pass data back and forth thanks to 
the little RTS hack.   We get a partial message back, so there is something
amiss.  Continue cleanup of I2C communications.  I am wondering if we can
use register reads to get access to specific pieces of information instead
of doing a tightly staged response.

Rewrite the communications a bit.  I figured out how to add a RTS signal 
back to the mcp.py so any data coming back can be fetched by the master 
without polling the slave to death.

In general, message passing will state the number of characters.   All 
messages will be two parts.  The message itself may have multiple commands 
separated by semicolon(;).  Semicolon ASCII = 59

* Part 1: (byte) message size (1-32 chars)
* Part 2: <message>

There are other problems with this, but let us keep it completely 
simple for now.

mcp.py is now a threaded program.  One thread is watching for keyboard 
input.  Will have to implement a daemon mode vs. interactive mode.

# 2017-05-22

Next:
* Make sure we can get data back to the mcp.py
* Begin rewite of GreenSNet.ino
* Consider wiring up digital signal wire to allow the mpaa library
to become a slave although it is always a master.  The goal is to listen
to a digitalPin.  We are adding a request to send signal to the I2C.

The command now makes it all the way in and obtains data!  Now to make
sure the data comes back out.
```
0000036755 [app] INFO: +cmdQ[1]:[wd1]
0000036756 [app] INFO: Processing command:wd1
0000036756 [app] INFO: >GreenSNet:[wd1]
0000038826 [app] INFO: +dataQ[1]:[$TP=83.5,HH=26.81,HT=83.5#]
0000038836 [app] INFO: Returning data:$TP=83.5,HH=26.81,HT=83.5#
0000058836 [app] INFO: +cmdQ[1]:[wd1]
0000058837 [app] INFO: Processing command:wd1
0000058837 [app] INFO: >GreenSNet:[wd1]
0000060907 [app] INFO: +dataQ[1]:[$TP=83.6,HH=26.79,HT=83.4#]
0000060917 [app] INFO: Returning data:$TP=83.6,HH=26.79,HT=83.4#
```

# 2017-05-21

Next:
* GreenMcp
  * Respond to command and send data back

Successfully passed a message from mcp.py to GreenMcp.ino.

Next:
* GreenMcp
  * I2C poll GreenXBee for pending commands
  * Allow us to put the electron into serial mode so we can easily update it via particle-cli
    * Build; signal and particle --serial flash program.bin

Created some program stubs.

Setup GreenMcp.ino
* Setup timer to poll GreenXBee over I2C for pending work
  * May want to setup a digital pin to set HIGH when there is work to be done?

Setup GreenSNet.ino
* Setup I2C interface and sensors

Setup GreenXBee.ino sketch (clone from HomeXBee.ino)
* Code will begin to diverge here as we customize behavior

HomeXBee
  Serial/I2C(mcp.py) -> cmdQ
    Remote command: XBee -> GreenXBee -> cmdQ
    Local  command: dataQ
  dataQ -> I2C(mcp.py)

GreenXBee
  Serial/XBee -> cmdQ
  cmdQ -> GreenMcp/I2C
  I2C -> dataQ
  dataQ -> XBee

GreenMcp
  cmdQ -> I2C
    Perform tasks, return data to dataQ
  dataQ -> I2C/GreenXBee

GreenSNet
  Sensors and data collection
  I2C slave

# 2017-05-20
Our first command from HomeXBee.ino made it to the greenhouse XBee.  Now
to get GreenXBee.ino started to respond.

Refactored HomeXBee.ino so the serial device can be easily changed without
changing a lot of code.  The python mraa library is a pain.  We are just
going to pass 32 bytes.  We can't read char by char as the readByte() 
function will just read a byte and trash the rest of the message.  We
might consider refactoring back to the python module smbus.   There is
a smbus2 library.  Will clean up the python code and post that soon.

Time to look at GreenXBee.ino code.  The goal to recieve a request and
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
* Arduino SparkFun RedBoard: GreenXBee.ino

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
