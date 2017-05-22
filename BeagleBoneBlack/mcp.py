#!/usr/bin/python

# Python library mraa documentation
# https://iotdk.intel.com/docs/master/mraa/python/index.html
# libmraa documentation
# https://iotdk.intel.com/docs/master/mraa/index.html
# Source: https://github.com/intel-iot-devkit/mraa


# I2C Configuration
# ADDRESS           DEVICE
# -------           ------
# <master>          BeagleBone Black Wireless
# 0x30              Arduino RedBoard/XBee
# 0x31              Particle Photon
# 0x32              Particle Electron (FUTURE)

import os, time, sys
import mraa as m

# This says we are MASTER
# We want BUS#2 
##
x = m.I2c(2,True)

# Set the device we want to talk to
# Address 0x30 (HomeXbee.ino)
##
x.address(0x30)

# Convert message to bytearray
msg = "mem\n"
msg = "ncmd\n"
msg = "mem\nncmd\nwd1\nncmd\nmem\n"
msg = "mem\nwd1\n"
msgarr = [ord(i) for i in msg]
#print len(msgarr)

# Send bytes
##
x.write(bytearray(msg))

# Go into the reader routine
# \n = 10
# \r = 13
##
nfail = 0
while nfail < 5:
  #print ">READ"
  data = x.read(32)
  msgfl = 0
  if len(data) == 32:
    msg = ""
    for i in data:
      #print i
      if i != 10 and i != 13 and i != 255:
        msg = msg + chr(i)
      if i == 10 or i == 13:
        if len(msg) > 0:
          if msg != "EOL":
            print "MSG:",msg
            msgfl = 1
            nfail = 0
            msg = ""
    if msgfl == 0 or msg == "EOL":
      time.sleep(1)
      nfail = nfail + 1

