################################################################################
import argparse
import logging
import queue
import sys
import threading
import time
import secrets
import datetime
import sys, re
import os
################################################################################
import mba

from mba import CanFrame

#import debugpy
#debugpy.listen(5678)

#uncomment this is deeper debug is needed   logging.getLogger().setLevel(logging.DEBUG)

os. system('CLS')

deviceCounterTarget = 6
aliveCounter = 0
deviceCounter = 0
deadCount = 0
canfd_payload = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]


def _device_callback(data):
  global aliveCounter
  global deviceCounter
  global deviceCounterTarget
  print("Tool Rx a Msg")
  if(data.command  == mba.CAN_MSG_RX):
    if(data.msg.busId == 1):
      #print("Rx Data %s" % hex(data.msg.id))
      if(data.msg.id == 0x100):
         print("Rx Data on channel 1 %s" % hex(data.msg.id))

  return

num_devices, types, serial_nums = mba.enum_devices()
if 0 >= num_devices:
  raise Exception('No devices detected.')

print("Found %d device" % num_devices)
print("Trying to open %s" % serial_nums[0])

device = mba.Mba()
instance = device.open_device(serial_nums[0])
if 0 > instance:
  logging.error("Failed to open device: %s" % str(instance))
  raise Exception('Failed open_device.')

rc = device.register_callback(_device_callback, None)

print("This is a tool test")    #should ALWAYS print
            
#################################################
#  Setup CAN-FD interface
#################################################
device.can_set_speed(mba.CAN0, 500, 2000)
device.can_set_speed(mba.CAN1, 1000, 2000)
#device.can_set_mode(mba.CAN0, mba.CAN_MODE_CLASSIC, mba.CAN_TESTMODE_NORMAL, False)
device.can_set_mode(mba.CAN1, mba.CAN_MODE_CLASSIC, mba.CAN_TESTMODE_NORMAL, False)
#device.can_set_mode(mba.CAN0, mba.CAN_MODE_FD, mba.CAN_TESTMODE_NORMAL, False)
#device.can_set_mode(mba.CAN1, mba.CAN_MODE_FD, mba.CAN_TESTMODE_NORMAL, False)

#canfd_id = 0x100
#canfd_id = 0x10A1238E
canfd_id = 0x8e
canfd_dlc = 8
canfd_payload[0] = 1
canfd_payload[1] = 2
canfd_payload[2] = 3
canfd_payload[3] = 4
canfd_payload[4] = 5
canfd_payload[5] = 6
canfd_payload[6] = 7
canfd_payload[7] = 8
       
#payload = [0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88]

#print(dir(mba))

#device.can_send_frame(mba.CAN0, canfd_id, canfd_payload, canfd_dlc, mba.CANFRAME_FLAG_NONE, 1000)
#device.can_send_frame(mba.CAN1, canfd_id, canfd_payload, canfd_dlc, mba.CANFRAME_FLAG_FD | mba.CANFRAME_FLAG_BRS, 1000)

#change the below flag, if you decide to use extended CAN
device.can_send_frame(mba.CAN1, canfd_id, canfd_payload, canfd_dlc, mba.CANFRAME_FLAG_NONE, 1000)
#device.can_send_frame(mba.CAN1, canfd_id, canfd_payload, canfd_dlc, mba.CANFRAME_FLAG_EXTENDED, 1000)

print("Transmitted message on channel 1:   ", canfd_id, canfd_payload, canfd_dlc)