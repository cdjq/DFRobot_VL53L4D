# -*- coding: utf-8 -*-
'''!
  @file DFRobot_VL53L4CD.py
  @brief Gravity / Fermion VL53L4CD I2C. The Raspberry Pi library does not implement UART.
  @copyright Copyright (c) 2026 DFRobot Co.Ltd (http://www.dfrobot.com)
  @license The MIT License (MIT)
  @author wxzed(xiao.wu@dfrobot.com)
  @version V1.0.0
  @date 2026-10-15
  @url https://github.com/DFRobot/DFRobot_VL53L4CD
'''

import time
try:
  from smbus2 import SMBus, i2c_msg
except ImportError:
  from smbus import SMBus
  i2c_msg = None
try:
  import RPi.GPIO as GPIO
except ImportError:
  GPIO = None

class VL53L4CDVersion(object):
  '''!
    @brief Library version, not the sensor firmware version.
  '''
  def __init__(self, major, minor, build, revision):
    self.major = major
    self.minor = minor
    self.build = build
    self.revision = revision

class VL53L4CDResult(object):
  '''!
    @brief One ranging sample. range_status 0 means distance is valid.
  '''
  def __init__(self):
    self.range_status = 0
    self.distance = 0
    self.number_of_spad = 0
    self.ambient_rate_kcps = 0
    self.signal_rate_kcps = 0
    self.ambient_per_spad_kcps = 0
    self.signal_per_spad_kcps = 0
    self.sigma_mm = 0

class DFRobot_VL53L4CD_I2C(object):
  '''!
    @brief I2C host for Gravity and Fermion. Register index is 16-bit big-endian.
    @n Firmware already runs SensorInit and StartRanging. This class does not replay that sequence.
  '''
  MODEL_ID = 0xEBAA
  REG_MODEL_ID = 0x010F
  REG_I2C_ADDR = 0x0001
  REG_OFFSET = 0x001E
  REG_INNER_OFFSET = 0x0020
  REG_OUTER_OFFSET = 0x0022
  REG_INTERRUPT = 0x0046
  REG_SIGMA_TH = 0x0064
  REG_SIGNAL_TH = 0x0066
  REG_THRESH_HIGH = 0x0072
  REG_THRESH_LOW = 0x0074
  REG_RANGE_STATUS = 0x0089
  REG_SPAD_NB = 0x008C
  REG_SIGNAL_RATE = 0x008E
  REG_AMBIENT_RATE = 0x0090
  REG_SIGMA = 0x0092
  REG_DISTANCE = 0x0096
  REG_SYSTEM_START = 0x0087
  REG_INT_CLEAR = 0x0086
  MAX_DISTANCE_MM = 1200
  LIB_VER_MAJOR = 1
  LIB_VER_MINOR = 0
  LIB_VER_BUILD = 0
  LIB_VER_REV = 0

  def __init__(self, bus=1, addr=0x52, xshut_pin=-1):
    '''!
      @brief Bind an I2C bus. xshut_pin -1 leaves the module enabled.
      @param bus I2C bus number, usually 1.
      @param addr 7-bit module address. Gravity must match the address DIP. Fermion ships at 0x52.
      @param xshut_pin BCM pin for a host-side XSHUT, or -1.
    '''
    self._bus_num = bus
    self._bus = None
    self._addr = addr
    self._xshut = xshut_pin
    self._gpio_ready = False

  def begin(self):
    '''!
      @brief Open I2C. If a host XSHUT pin was given, drive it high first.
      @return True on success, False if smbus2 is missing or the bus cannot open.
    '''
    if i2c_msg == None:
      return False
    self.power_on()
    try:
      self._bus = SMBus(self._bus_num)
    except Exception:
      self._bus = None
      return False
    return True

  def power_on(self):
    '''!
      @brief Drive the host XSHUT pin high. No pin means no GPIO change.
    '''
    if self._xshut < 0 or GPIO == None:
      return
    if self._gpio_ready == False:
      GPIO.setmode(GPIO.BCM)
      GPIO.setwarnings(False)
      self._gpio_ready = True
    GPIO.setup(self._xshut, GPIO.OUT)
    GPIO.output(self._xshut, GPIO.HIGH)

  def power_off(self):
    '''!
      @brief Drive the host XSHUT pin low. No pin means no GPIO change.
    '''
    if self._xshut < 0 or GPIO == None:
      return
    if self._gpio_ready == False:
      GPIO.setmode(GPIO.BCM)
      GPIO.setwarnings(False)
      self._gpio_ready = True
    GPIO.setup(self._xshut, GPIO.OUT)
    GPIO.output(self._xshut, GPIO.LOW)

  def init_sensor(self, addr=0x52):
    '''!
      @brief Select the 7-bit address and confirm MODEL_ID is 0xEBAA.
      @param addr Ignored after the address is stored. UART is not used here.
      @return True when the model id matches.
    '''
    self._on_init_address(addr)
    return self.sensor_init()

  def sensor_init(self):
    '''!
      @brief Confirm the sensor can be read by checking model id 0xEBAA.
      @return True when MODEL_ID is 0xEBAA.
    '''
    sensor_id = self.get_sensor_id()
    if sensor_id == None or sensor_id != self.MODEL_ID:
      return False
    return True

  def get_version(self):
    '''!
      @brief Read this library version.
      @return VL53L4CDVersion.
    '''
    return VL53L4CDVersion(self.LIB_VER_MAJOR, self.LIB_VER_MINOR,
                           self.LIB_VER_BUILD, self.LIB_VER_REV)

  def get_sensor_id(self):
    '''!
      @brief Read register 0x010F.
      @return Device id, or None on a bus error.
    '''
    return self._read_reg16(self.REG_MODEL_ID)

  def start_ranging(self):
    '''!
      @brief Write 0x21 to 0x0087. Firmware already started ranging.
      @return True on success.
    '''
    return self._write_reg8(self.REG_SYSTEM_START, 0x21)

  def stop_ranging(self):
    '''!
      @brief Write 0x80 to 0x0087.
      @return True on success.
    '''
    return self._write_reg8(self.REG_SYSTEM_START, 0x80)

  def get_result(self):
    '''!
      @brief Read the result block.
      @return VL53L4CDResult, or None on a bus error. range_status 0 is a valid distance.
    '''
    block = self._read_result_block(24)
    if block == None:
      return None
    result = VL53L4CDResult()
    off_dist = self.REG_DISTANCE - self.REG_RANGE_STATUS
    off_sig = self.REG_SIGNAL_RATE - self.REG_RANGE_STATUS
    off_amb = self.REG_AMBIENT_RATE - self.REG_RANGE_STATUS
    off_sigma = self.REG_SIGMA - self.REG_RANGE_STATUS
    off_spad = self.REG_SPAD_NB - self.REG_RANGE_STATUS
    result.range_status = self._map_range_status(block[0])
    raw_spads = (block[off_spad] << 8) | block[off_spad + 1]
    result.number_of_spad = raw_spads // 256
    result.distance = (block[off_dist] << 8) | block[off_dist + 1]
    word = (block[off_sig] << 8) | block[off_sig + 1]
    result.signal_rate_kcps = word * 8
    word = (block[off_amb] << 8) | block[off_amb + 1]
    result.ambient_rate_kcps = word * 8
    word = (block[off_sigma] << 8) | block[off_sigma + 1]
    result.sigma_mm = word // 4
    if raw_spads == 0:
      result.signal_per_spad_kcps = 0
      result.ambient_per_spad_kcps = 0
    else:
      result.signal_per_spad_kcps = result.signal_rate_kcps * 256 // raw_spads
      result.ambient_per_spad_kcps = result.ambient_rate_kcps * 256 // raw_spads
    return result

  def set_offset(self, offset_mm):
    '''!
      @brief Write the distance offset in millimetres. Range is -1024 to 1023.
      @return True on success.
    '''
    if offset_mm < -1024 or offset_mm > 1023:
      return False
    temp = (offset_mm & 0xFFFF) * 4
    temp = temp & 0xFFFF
    if self._write_reg16(self.REG_OFFSET, temp) == False:
      return False
    if self._write_reg16(self.REG_INNER_OFFSET, 0) == False:
      return False
    if self._write_reg16(self.REG_OUTER_OFFSET, 0) == False:
      return False
    return True

  def get_offset(self):
    '''!
      @brief Read the distance offset in millimetres.
      @return Offset, or None on a bus error.
    '''
    temp = self._read_reg16(self.REG_OFFSET)
    if temp == None:
      return None
    temp = ((temp << 3) & 0xFFFF) >> 5
    if temp > 1024:
      temp = temp - 2048
    return temp

  def calibrate_offset(self, target_mm, nb_samples):
    '''!
      @brief Clear the offset, average valid ranges, then write target minus that average.
      @param target_mm Known target distance.
      @param nb_samples Number of attempts. Invalid ranges are skipped.
      @return Measured offset in millimetres, or None on failure.
    '''
    if nb_samples <= 0 or self.set_offset(0) == False:
      return None
    total = 0
    got = 0
    i = 0
    while i < nb_samples:
      time.sleep(0.05)
      result = self.get_result()
      if result != None and result.range_status == 0:
        total += result.distance
        got += 1
      i += 1
    if got == 0:
      return None
    offset = int(target_mm) - (total // got)
    if self.set_offset(offset) == False:
      return None
    return self.get_offset()

  def set_signal_threshold(self, signal_kcps):
    '''!
      @brief Set the return-signal threshold in kcps. Maximum 16384.
      @return True on success.
    '''
    if signal_kcps > 16384:
      return False
    return self._write_reg16(self.REG_SIGNAL_TH, signal_kcps >> 3)

  def get_signal_threshold(self):
    '''!
      @brief Read the return-signal threshold in kcps.
      @return Threshold, or None on a bus error.
    '''
    temp = self._read_reg16(self.REG_SIGNAL_TH)
    if temp == None:
      return None
    return (temp << 3) & 0xFFFFFFFF

  def set_sigma_threshold(self, sigma_mm):
    '''!
      @brief Set the sigma threshold in millimetres. Maximum 16383.
      @return True on success.
    '''
    if sigma_mm > (0xFFFF >> 2):
      return False
    return self._write_reg16(self.REG_SIGMA_TH, (sigma_mm << 2) & 0xFFFF)

  def get_sigma_threshold(self):
    '''!
      @brief Read the sigma threshold in millimetres.
      @return Threshold, or None on a bus error.
    '''
    temp = self._read_reg16(self.REG_SIGMA_TH)
    if temp == None:
      return None
    return temp >> 2

  def set_detection_thresholds(self, low_mm, high_mm, window):
    '''!
      @brief Set the GPIO1 distance window. window is 0..3.
      @return True on success.
    '''
    if window > 3:
      return False
    if self._write_reg8(self.REG_INTERRUPT, window) == False:
      return False
    if self._write_reg16(self.REG_THRESH_HIGH, high_mm) == False:
      return False
    if self._write_reg16(self.REG_THRESH_LOW, low_mm) == False:
      return False
    return True

  def get_detection_thresholds(self):
    '''!
      @brief Read the GPIO1 distance window.
      @return (low_mm, high_mm, window), or None on a bus error.
    '''
    high_mm = self._read_reg16(self.REG_THRESH_HIGH)
    low_mm = self._read_reg16(self.REG_THRESH_LOW)
    window = self._read_reg8(self.REG_INTERRUPT)
    if high_mm == None or low_mm == None or window == None:
      return None
    return (low_mm, high_mm, window & 0x7)

  def set_i2c_address(self, addr):
    '''!
      @brief Write the 7-bit address to 0x0001, then read 0x0001 back at the new address.
      @n Fermion changes the address and keeps it in flash. Gravity ignores the write and
      @n keeps the DIP address, so the read-back fails and this returns False.
      @return True when the module answers at the new address.
    '''
    if addr < 0x08 or addr > 0x77:
      return False
    if self._write_reg8(self.REG_I2C_ADDR, addr) == False:
      return False
    # The module ACKs the write first and applies it later in its main loop.
    time.sleep(0.05)
    old_addr = self._addr
    self._addr = addr
    if self._read_reg8(self.REG_I2C_ADDR) == addr:
      return True
    self._addr = old_addr
    return False

  def _on_init_address(self, addr):
    self._addr = addr

  def _map_range_status(self, raw):
    status_rtn = [
        255, 255, 255, 5, 2, 4, 1, 7, 3, 0, 255, 255, 9, 13, 255, 255,
        255, 255, 10, 6, 255, 255, 11, 12]
    raw = raw & 0x1F
    if raw < 24:
      return status_rtn[raw]
    return 255

  def _write_reg8(self, reg, value):
    return self._write_bytes(reg, [value & 0xFF])

  def _write_reg16(self, reg, value):
    return self._write_bytes(reg, [(value >> 8) & 0xFF, value & 0xFF])

  def _read_reg8(self, reg):
    data = self._read_bytes(reg, 1)
    if data == None:
      return None
    return data[0]

  def _read_reg16(self, reg):
    data = self._read_bytes(reg, 2)
    if data == None:
      return None
    return (data[0] << 8) | data[1]

  def _read_result_block(self, length):
    return self._read_bytes(self.REG_RANGE_STATUS, length)

  def _write_bytes(self, reg, data):
    if self._bus == None:
      return False
    try:
      msg = i2c_msg.write(self._addr, [(reg >> 8) & 0xFF, reg & 0xFF] + data)
      self._bus.i2c_rdwr(msg)
    except Exception:
      return False
    return True

  def _read_bytes(self, reg, length):
    if self._bus == None or length <= 0:
      return None
    try:
      write = i2c_msg.write(self._addr, [(reg >> 8) & 0xFF, reg & 0xFF])
      read = i2c_msg.read(self._addr, length)
      self._bus.i2c_rdwr(write, read)
    except Exception:
      return None
    return list(read)
