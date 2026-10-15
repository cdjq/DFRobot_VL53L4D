# -*- coding: utf-8 -*-
'''!
  @file calibrate_offset.py
  @brief Calibrate offset against a stable target at a known distance.
  @n Raspberry Pi: SDA=GPIO2, SCL=GPIO3.
  @copyright Copyright (c) 2026 DFRobot Co.Ltd
'''
import sys
import time
sys.path.append("../../")
from DFRobot_VL53L4CD import DFRobot_VL53L4CD_I2C

TARGET_MM = 100

def setup():
  '''!
    @brief Average valid ranges and store the offset.
  '''
  global tof
  tof = DFRobot_VL53L4CD_I2C(bus=1, addr=0x52, xshut_pin=-1)
  if tof.begin() == False or tof.init_sensor(0x52) == False:
    print("init failed")
    while True:
      time.sleep(1)
  print("hold a stable target, calibrating...")
  offset = tof.calibrate_offset(TARGET_MM, 20)
  if offset == None:
    print("calibrate failed")
    while True:
      time.sleep(1)
  print("offset mm=%d" % offset)

def loop():
  '''!
    @brief Print valid distances after calibration.
  '''
  result = tof.get_result()
  if result != None and result.range_status == 0:
    print("mm=%u" % result.distance)
  time.sleep(0.2)

if __name__ == "__main__":
  setup()
  while True:
    loop()
