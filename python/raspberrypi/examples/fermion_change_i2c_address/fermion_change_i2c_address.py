# -*- coding: utf-8 -*-
'''!
  @file fermion_change_i2c_address.py
  @brief Fermion stores a new 7-bit address at register 0x0001.
  @n Gravity ignores the call and keeps the DIP address.
  @n Raspberry Pi: SDA=GPIO2, SCL=GPIO3.
  @copyright Copyright (c) 2026 DFRobot Co.Ltd
'''
import sys
import time
sys.path.append("../../")
from DFRobot_VL53L4CD import DFRobot_VL53L4CD_I2C

def setup():
  '''!
    @brief Change a Fermion module from 0x52 to 0x53.
  '''
  tof = DFRobot_VL53L4CD_I2C(bus=1, addr=0x52, xshut_pin=-1)
  if tof.begin() == False or tof.init_sensor(0x52) == False:
    print("init failed")
    while True:
      time.sleep(1)
  if tof.set_i2c_address(0x53) == True:
    print("Fermion address saved as 0x53")
    sensor_id = tof.get_sensor_id()
    if sensor_id != None:
      print("Sensor ID: 0x%04X" % sensor_id)
  else:
    print("rejected: Gravity uses the address DIP, not set_i2c_address")

def loop():
  '''!
    @brief Address change runs once in setup.
  '''
  time.sleep(1)

if __name__ == "__main__":
  setup()
  while True:
    loop()
