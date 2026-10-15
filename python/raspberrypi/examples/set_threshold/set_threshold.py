# -*- coding: utf-8 -*-
'''!
  @file set_threshold.py
  @brief Set the GPIO1 distance window and read it back.
  @n Raspberry Pi: SDA=GPIO2, SCL=GPIO3. Set the DIP switches to I2C.
  @copyright Copyright (c) 2026 DFRobot Co.Ltd
'''
import sys
import time
sys.path.append("../../")
from DFRobot_VL53L4CD import DFRobot_VL53L4CD_I2C

def setup():
  '''!
    @brief Program a 100..300 mm inside window.
  '''
  global tof
  tof = DFRobot_VL53L4CD_I2C(bus=1, addr=0x52, xshut_pin=-1)
  if tof.begin() == False or tof.init_sensor(0x52) == False:
    print("init failed")
    while True:
      time.sleep(1)
  if tof.set_detection_thresholds(100, 300, 3) == False:
    print("set threshold failed")
    while True:
      time.sleep(1)
  window = tof.get_detection_thresholds()
  if window != None:
    print("window=%u low=%u high=%u" % (window[2], window[0], window[1]))

def loop():
  '''!
    @brief Print distance and range status.
  '''
  result = tof.get_result()
  if result != None:
    print("mm=%u status=%u" % (result.distance, result.range_status))
  time.sleep(0.2)

if __name__ == "__main__":
  setup()
  while True:
    loop()
