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

# REGISTERS
# ADDR:0x30
# REG#       SZ     RESPONSE
# ----       --     --------
# readBytesReg(REG,SZ): Read only
# 0x01        1     Return # of msgs in selected input queue
# 0x02        1     Return # of msgs in selected output queue
# 0x03        1     Input queue select (1=Commands;2=Data)
# 0x04        1     Output queue select (1=Commands;2=Data)
# 0x05        1     Get next message from selected output queue (1=OK; 2=FAIL)
# 0x06        1     Get size of current output message
# 0x07        x     Fetch current message with size x 
# 0x08        1     Output message sequence
#                     1 = old message
#                     2 = new message was pulled from selected queue
#                     3 = size was requested
#                     Once a message is read, status returns to 1.
#
# A response of 0 may indicate an I/O error
# CONSTANTS FOR ABOVE:
##
regs = {
  "NMSG_INPUT": 1,
  "NMSG_OUTPUT": 2,
  "QSEL_INPUT": 3,
  "QSEL_OUTPUT": 4,
  "GET_NEXT_MSG": 5,
  "GET_MSG_SIZE": 6,
  "FETCH_MSG": 7,
  "GET_MSG_SEQ": 8,
}

# Register write does not support bytearray input so use plain
# write.  Strings need to be greater than one byte.
# write(DATA)
#   DATA will be added to the queue specified by REG 0x07 
##

# VALID COMMANDS
# COMMAND           DESCRIPTION
# mem               Show free memory
# cqcmd             Clear command queue
# cqdata            Clear data queue
# icmd              Set input queue to Command
# idata             Set input queue to Data
# ocmd              Set output queue to Command
# odata             Set output queue to Data
# ping1             Ping HomeXBee
# ping2             Ping GreenXBee
# ping3             Ping GreenSNet
# ping4             Ping GreenMcp
# r[1-4]{on|off}    Turn relay #[1-4] {on|off}
# rstat             Get relay status
# wd1               Weather block  1:
# gp1               GPS block      1:
##

# OUTPUT
##
# In general strings that start with ($) and end with (#)
# are data values from sensors or devices.   In some
# cases other strings in the Data queue may be re-interpreted
# as Commands.  Sensor and data readings are described in
# a separate document.
##

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
#   Rewrite communication code to use register responses
#   Get wd1 to return data
#   daemon vs. interactive mode
#   Reading of GPIO pin is hardcoded for now.  We should create a
#   library/class wrapper to extend mraa so there are extra
#   functions:
#     mraa.available() [size of next message; 0 otherwise]
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

# Return available data like the Arduino available() function
##
def data_available():
  sz = 0

  # If RTS is LOW, there is data ready to send from
  # currently selected output queue.  We let the data
  # reader take care if this is really an old message.
  ##
  if GPIO.input(i2c_rts_pin) == 0:
    ret = i2c.readBytesReg(reg["GET_MSG_SIZE"],1)
    sz = ord(ret)
    if sz > 32: sz = 0

  return sz

# Fetch next message out of the currently selected
# output queue.
def fetch_data():
  # Confirm message sequence, we might need to
  # re-read an older message in case of I/O
  # error.
  ##
  ret = i2c.readBytesReg(reg["GET_MSG_SEQ"],1)
  seq = ord(ret)
  if seq == 1:
    # This is an old message, request the next one in the queue
    ##
    ret = i2c.readBytesReg(reg["GET_NEXT_MSG",1)
    ret = ord(ret)
    if ret == 0:
      # I/O error, exit
      return
    seq = 2
    
  # Fetch # of bytes in the current message
  ##
  sz = 0
  if seq == 2:
    ret = i2c.readBytesReg(reg["GET_MSG_SIZE",1)
    ret = ord(ret)
    if ret == 0:
      # I/O error, exit
      return
    sz = ret
    seq = 3

  # Attempt to fetch message
  if seq == 3 and sz > 0:
    ret = i2c.readBytesReg(reg["FETCH_MSG"],sz)
    print ">",ret


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

# Main thread: loop()
##
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
      #i2c.write(bytearray([lstr]))
      i2c.write(bytearray([ord(i) for i in kbdInput]))
    activity = True
  # Check to see if slave has data
  ##
  if data_available() > 0:
    fetch_data()

  if activity == False:
    time.sleep(0.1)

