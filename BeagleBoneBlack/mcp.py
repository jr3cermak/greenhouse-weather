#!/usr/bin/python

# Python library mraa documentation
# https://iotdk.intel.com/docs/master/mraa/python/index.html
# libmraa documentation
# https://iotdk.intel.com/docs/master/mraa/index.html
# Source: https://github.com/intel-iot-devkit/mraa
##

# Module imports
##

# Create a threaded python script
# Main thread work with HomeXBee
#   Subthread collects keyboard input
##
import threading

# Standard python modules
##
import os, time, sys
import argparse

# mraa allows use of I2C
##
import mraa as m

# Helps us setup the RTS line to listen for data
# at the slave since BBBW can only act as a I2C Master.
##
import Adafruit_BBIO.GPIO as GPIO

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
#                     0 = no message or I/O error
#                     1 = next message was requested
#                     2 = size of message was requested
#                     3 = attempt was made to retrieve message
#                     If client was successful, please send ACK by
#                     sending a request to ADDR 0x08
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
lastReg = 0
msgSeq = {
  "MSG_SEQ_IO": 0,
  "MSG_SEQ_NEXTMSG": 1,
  "MSG_SEQ_SIZE": 2,
  "MSG_SEQ_GETMSG": 3,
}
lastMsgSeq = 0

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
#   Detect when RTS goes LOW but the XBee is really offline, don't crash
#   Logging data
#   daemon vs. interactive mode
#   Reading of GPIO pin is hardcoded for now.  We should create a
#   library/class wrapper to extend mraa so there are extra
#   functions:
#     mraa.available() [size of next message; 0 otherwise]
#     mraa.setRTS(pin) [set a gpio pin to watch]
#     mraa.getRTS()    [indicate if a pin has been set]
#     mraa.resetRTS()  [attempt to reset the backend GPIO]
##

# Valid arguments to mcp.py
##
parser = argparse.ArgumentParser()
parser.add_argument("-c", help="send command")
parser.add_argument("-d", help="deamon mode",action="store_true")
parser.add_argument("-k", help="enable keyboard input", action="store_true")
parser.add_argument("-r", help="run once to download any messages", action="store_true")
args = parser.parse_args()

# Globals
##
HomeXBee = 0x30
i2c_rts_pin = "P8_11"
kbdInputRaw = ""
kbdInput = ""
kbdReady = False
loopCt = 0
ioErrors = 0
dataCt = 0

# Functions
##
def kbdListener():
  global kbdInput, kdbInputRaw, kbdReady
  print "Ready for keyboard input"
  while True:
    kbdInputRaw = raw_input()
    kbdInput = kbdInputRaw
    kbdReady = True
    if kbdInput == "exit":
      break

# This should just return true or false if the slave
# thinks it has data.
##
def data_available():
  # If RTS is LOW, there is data ready to send from
  # currently selected output queue.
  ##
  if GPIO.input(i2c_rts_pin) == 0:
    return True

  return False

# Fetch next message out of the currently selected
# output queue.
def fetchData():
  global ioErrors, dataCt
  # Confirm message sequence, we might need to
  # re-read an older message in case of an I/O
  # error.
  ##
  ret = 0
  seq = 0
  sz = 0
  # We have seen IO errors here before
  ##
  try:
    ret = i2c.readBytesReg(regs["GET_MSG_SEQ"],1)
  except:
    ioErrors = ioErrors + 1
    return

  seq = ord(ret)
  if seq <= 1:
    # This is an old message, request the next one in the queue
    ##
    ret = i2c.readBytesReg(regs["GET_NEXT_MSG"],1)
    ret = ord(ret)
    if ret == 0:
      # I/O error, exit
      return
    if ret == 1:
      seq = 2
    if ret == 2:
      # Failed, do something different here
      ret = 2
    
  # Fetch # of bytes in the current message
  # This needs to be run if seq == 3 since size is needed
  ##
  sz = 0
  if seq >= 2:
    ret = i2c.readBytesReg(regs["GET_MSG_SIZE"],1)
    ret = ord(ret)
    if ret == 0:
      # I/O error, exit
      return
    sz = ret
    if sz == 0:
      return
    seq = 3

  # Attempt to fetch message
  if seq == 3 and sz > 0:
    ret = i2c.readBytesReg(regs["FETCH_MSG"],sz)
    if (ret == 0):
      # I/O error
      return;
    print "<",ret
    dataCt = dataCt + 1
    ret = i2c.readBytesReg(regs["GET_MSG_SEQ"],1)

# Start keyboard reader thread
##
if args.k == True:
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

# If we are running once and have a command
# add it to keyboard buffer for sending.
##
if args.r == True and args.c != None:
  kbdReady = True
  kbdInput = args.c

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
      print "sent>",kbdInput,"len(",lstr,")"
      # Send parts 1 and 2
      ##
      #i2c.write(bytearray([lstr]))
      i2c.write(bytearray([ord(i) for i in kbdInput]))
    activity = True
  # Check to see if slave has data
  ##
  if data_available() > 0:
    fetchData()

  if activity == False:
    loopCt = loopCt + 1
    time.sleep(0.1)

    # if we are half way through and we have not seen data, try again
    ##
    if args.r == True and args.c != None and loopCt == 150 and dataCt < 2:
      kbdReady = True
      kbdInput = args.c

  # Run for 30 sec
  ##
  if args.r == True and loopCt > 300:
    sys.exit()

