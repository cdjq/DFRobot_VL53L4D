# -*- coding: utf-8 -*-
'''!
  @file get_distance.py
  @brief Gravity VL53L4CD continuous ranging over I2C.
  @n Enable I2C and set the address DIP to match MODULE_I2C_ADDR.
  @n Raspberry Pi: SDA=GPIO2, SCL=GPIO3.
  @copyright Copyright (c) 2026 DFRobot Co.Ltd
'''
import sys
import time
sys.path.append("../../")
from DFRobot_VL53L4CD import DFRobot_VL53L4CD_I2C

MODULE_I2C_ADDR = 0x52

def setup():
  '''!
    @brief Open I2C and confirm the model id.
  '''
  global tof
  tof = DFRobot_VL53L4CD_I2C(bus=1, addr=MODULE_I2C_ADDR, xshut_pin=-1)
  print("VL53L4CD I2C getDistance")
  if tof.begin() == False:
    print("begin failed")
    while True:
      time.sleep(1)
  if tof.init_sensor(MODULE_I2C_ADDR) == False:
    print("init_sensor failed (check addr / wiring / I2C mode)")
    while True:
      time.sleep(1)
  sensor_id = tof.get_sensor_id()
  if sensor_id != None:
    print("Sensor ID: 0x%04X" % sensor_id)
  print("Ranging...")

def loop():
  '''!
    @brief Print one sample. range_status 0 is a valid distance.
  '''
  result = tof.get_result()
  if result != None:
    prefix = "mm" if result.range_status == 0 else "invalid mm"
    print("%s=%u status=%u signal=%u" % (
        prefix, result.distance, result.range_status, result.signal_rate_kcps))
  else:
    print("I2C read fail")
  time.sleep(0.2)

if __name__ == "__main__":
  setup()
  while True:
    loop()
