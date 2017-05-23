#!/usr/bin/python

# Python library mraa documentation
# https://iotdk.intel.com/docs/master/mraa/python/index.html
# libmraa documentation
# https://iotdk.intel.com/docs/master/mraa/index.html
# Source: https://github.com/intel-iot-devkit/mraa
##

# BBBW
# /sys/kernel/debug/pinctrl/44e10800.pinmux/pinmux-pins
# pin 13 (44e10834.0): ocp:P8_11_pinmux (GPIO UNCLAIMED) function
#                      pinmux_P8_11_gpio_pin group pinmux_P8_11_gpio_pin
##

# I2C Configuration
# ADDRESS           DEVICE
# -------           ------
# <master>          BeagleBone Black Wireless
# 0x30              Arduino RedBoard/XBee (HomeXBee)
#  RTS              BBBW (P8_11) -> HomeXBee (D12)
# 0x31              Particle Photon
# 0x32              Particle Electron (FUTURE)

# RTS (request to send) wiring notes
# P8_11 is connected to a pulldown resistor and path continues
# through a level shifter (Adafruit I2C-safe bidirectional Logic Level
# Converter Part#757) (A4) -> (B4) -> Arduino D12. 
# The P8_11 is set as an INPUT.
# D12 is set as a digital output with default to HIGH.  The RTS is
# pulled LOW when data is ready on the HomeXBee.   The RTS signal helps
# get around a problem of the python mraa library where it can not act
# as a slave device.  We simulate the available() function by testing
# the RTS signal.
# We also have to use the Adafruit_BBIO library as the mraa also seems
# to lack BBBW pin support via the Gpio function.
##

# TODO:
#   daemon vs. interactive mode
#   Reading of GPIO pin is hardcoded for now.  We should create a
#   library/class wrapper to extend mraa so there are extra
#   functions:
#     mraa.available() [true/false; returns false if pin is not set]
#     mraa.setRTS(pin) [set a gpio pin to watch]
#     mraa.getRTS()    [indicate if a pin has been set]
#     mraa.resetRTS()  [attempt to reset the backend GPIO]
##

import threading
import os, time, sys
import mraa as m
import Adafruit_BBIO.GPIO as GPIO

# Create a threaded python script
# Main thread work with HomeXBee
#   Subthread collects keyboard input
##

# Globals
##
HomeXBee = 0x30
i2c_rts_pin = "P8_11"
kbdInputRaw = ""
kbdInput = ""
kbdReady = False

# Functions
##
def kbdListener():
  global kbdInput, kdbInputRaw, kbdReady
  print "Ready for input"
  while True:
    kbdInputRaw = raw_input()
    kbdInput = kbdInputRaw
    kbdReady = True
    if kbdInput == "exit":
      break

# Convert message to bytearray
#msg = "mem\n"
#msg = "ncmd\n"
#msg = "mem\nncmd\nwd1\nncmd\nmem\n"
#msg = "mem\nwd1\n"
#msgarr = [ord(i) for i in msg]
#print len(msgarr)

# Send bytes
##
#i2c.write(bytearray(msg))

def data_available():
  ret = False

  if GPIO.input(i2c_rts_pin) == 0:
    ret = True

  return ret

def fetch_data():
  # Part 0 (initiate data request)
  ##
  lstr = 33
  i2c.write(bytearray([lstr]))
  # Part 1
  ##
  nbytes = i2c.read(1)
  if ord(nbytes) == 0:
    # No data?
    ##
    return

  print ">",ord(nbytes)
  # Part 2
  ##
  data = i2c.read(ord(nbytes))
  print ">",data

# Start keyboard reader thread
##
listener = threading.Thread(target=kbdListener)
listener.start()

# Setup GPIO pin for input
##
GPIO.setup(i2c_rts_pin,GPIO.IN)

# Connect to I2C2 bus
# True = raw mode
##
i2c = m.I2c(2,True)

# Set the device we want to talk to
# Address 0x30 (HomeXbee.ino)
##
i2c.address(HomeXBee)

# Go into main thread loop
# \n = 10
# \r = 13
##
idlect = 0
while True:
  activity = False
  if kbdReady:
    kbdReady = False
    #print "kbd>",kbdInput
    if kbdInput == "exit":
      break
    else:
      lstr = len(kbdInput)
      print ">",kbdInput,lstr
      # Send parts 1 and 2
      ##
      i2c.write(bytearray([lstr]))
      i2c.write(bytearray([ord(i) for i in kbdInput]))
    activity = True
  # Check to see if slave has data
  ##
  if data_available():
    fetch_data()

  if activity == False:
    time.sleep(0.1)

sys.exit()
while True:
  data = i2c.read(32)
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

