/*!
 * @file DFRobot_VL53L4CD.cpp
 * @brief VL53L4CD I2C 实现（16 位寄存器指针，大端）
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-09
 */
#include "DFRobot_VL53L4CD.h"

#define LIB_VER_MAJOR 1u
#define LIB_VER_MINOR 0u
#define LIB_VER_BUILD 0u
#define LIB_VER_REV 0u

DFRobot_VL53L4CD_I2C::DFRobot_VL53L4CD_I2C(TwoWire *i2c, int xshut_pin)
    : _i2c(i2c), _xshut(xshut_pin), _addr(0x52u) {}

void DFRobot_VL53L4CD_I2C::powerOn(void)
{
  if (_xshut >= 0) {
    pinMode((uint8_t)_xshut, OUTPUT);
    digitalWrite((uint8_t)_xshut, HIGH);
  }
}

void DFRobot_VL53L4CD_I2C::powerOff(void)
{
  if (_xshut >= 0) {
    pinMode((uint8_t)_xshut, OUTPUT);
    digitalWrite((uint8_t)_xshut, LOW);
  }
}

int DFRobot_VL53L4CD_I2C::begin(void)
{
  if (_i2c == nullptr) {
    return 0;
  }
  powerOn();
  _i2c->begin();
  return 1;
}

bool DFRobot_VL53L4CD_I2C::writeReg8(uint16_t reg, uint8_t value)
{
  _i2c->beginTransmission(_addr);
  _i2c->write((uint8_t)(reg >> 8));
  _i2c->write((uint8_t)(reg & 0xFFu));
  _i2c->write(value);
  return _i2c->endTransmission() == 0;
}

bool DFRobot_VL53L4CD_I2C::writeReg16(uint16_t reg, uint16_t value)
{
  _i2c->beginTransmission(_addr);
  _i2c->write((uint8_t)(reg >> 8));
  _i2c->write((uint8_t)(reg & 0xFFu));
  _i2c->write((uint8_t)(value >> 8));
  _i2c->write((uint8_t)(value & 0xFFu));
  return _i2c->endTransmission() == 0;
}

bool DFRobot_VL53L4CD_I2C::readReg8(uint16_t reg, uint8_t *value)
{
  if (value == nullptr) {
    return false;
  }
  _i2c->beginTransmission(_addr);
  _i2c->write((uint8_t)(reg >> 8));
  _i2c->write((uint8_t)(reg & 0xFFu));
  if (_i2c->endTransmission(false) != 0) {
    return false;
  }
  if (_i2c->requestFrom((int)_addr, 1) != 1) {
    return false;
  }
  *value = (uint8_t)_i2c->read();
  return true;
}

bool DFRobot_VL53L4CD_I2C::readReg16(uint16_t reg, uint16_t *value)
{
  uint8_t buf[2];

  if (value == nullptr) {
    return false;
  }
  _i2c->beginTransmission(_addr);
  _i2c->write((uint8_t)(reg >> 8));
  _i2c->write((uint8_t)(reg & 0xFFu));
  if (_i2c->endTransmission(false) != 0) {
    return false;
  }
  if (_i2c->requestFrom((int)_addr, 2) != 2) {
    return false;
  }
  buf[0] = (uint8_t)_i2c->read();
  buf[1] = (uint8_t)_i2c->read();
  *value = (uint16_t)(((uint16_t)buf[0] << 8) | buf[1]);
  return true;
}

bool DFRobot_VL53L4CD_I2C::readReg32(uint16_t reg, uint32_t *value)
{
  uint8_t buf[4];

  if (value == nullptr) {
    return false;
  }
  _i2c->beginTransmission(_addr);
  _i2c->write((uint8_t)(reg >> 8));
  _i2c->write((uint8_t)(reg & 0xFFu));
  if (_i2c->endTransmission(false) != 0) {
    return false;
  }
  if (_i2c->requestFrom((int)_addr, 4) != 4) {
    return false;
  }
  for (uint8_t i = 0; i < 4u; i++) {
    buf[i] = (uint8_t)_i2c->read();
  }
  *value = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) |
           buf[3];
  return true;
}

int DFRobot_VL53L4CD_I2C::InitSensor(uint8_t addr)
{
  uint16_t id = 0;

  _addr = addr;
  if (!readReg16(VL53L4CD_REG_MODEL_ID, &id)) {
    return 0;
  }
  if (id != VL53L4CD_MODEL_ID) {
    return 0;
  }
  return 1;
}

int DFRobot_VL53L4CD_I2C::getVersion(VL53L4CD_Version_t *pVersion)
{
  if (pVersion == nullptr) {
    return 0;
  }
  pVersion->major = LIB_VER_MAJOR;
  pVersion->minor = LIB_VER_MINOR;
  pVersion->build = LIB_VER_BUILD;
  pVersion->revision = LIB_VER_REV;
  return 1;
}

int DFRobot_VL53L4CD_I2C::getSensorId(uint16_t *p_id)
{
  if (p_id == nullptr || !readReg16(VL53L4CD_REG_MODEL_ID, p_id)) {
    return 0;
  }
  return 1;
}

int DFRobot_VL53L4CD_I2C::startRanging(void)
{
  if (!writeReg8(VL53L4CD_REG_SYSTEM_START, 0x40u)) {
    return 0;
  }
  return 1;
}

int DFRobot_VL53L4CD_I2C::stopRanging(void)
{
  if (!writeReg8(VL53L4CD_REG_SYSTEM_START, 0x00u)) {
    return 0;
  }
  return 1;
}

/* 与 ST VL53L4CD_GetResult 相同的 range_status 映射 */
uint8_t DFRobot_VL53L4CD_I2C::mapRangeStatus(uint8_t raw)
{
  static const uint8_t status_rtn[24] = {
      255, 255, 255, 5, 2, 4, 1, 7, 3, 0, 255, 255, 9, 13, 255, 255,
      255, 255, 10, 6, 255, 255, 11, 12};

  raw &= 0x1Fu;
  if (raw < 24u) {
    return status_rtn[raw];
  }
  return 255u;
}

bool DFRobot_VL53L4CD_I2C::readResultBlock(uint8_t *buf, uint8_t len)
{
  if (buf == nullptr || len == 0u) {
    return false;
  }
  _i2c->beginTransmission(_addr);
  _i2c->write((uint8_t)(VL53L4CD_REG_RANGE_STATUS >> 8));
  _i2c->write((uint8_t)(VL53L4CD_REG_RANGE_STATUS & 0xFFu));
  if (_i2c->endTransmission(false) != 0) {
    return false;
  }
  if (_i2c->requestFrom((int)_addr, (int)len) != (int)len) {
    return false;
  }
  for (uint8_t i = 0; i < len; i++) {
    buf[i] = (uint8_t)_i2c->read();
  }
  return true;
}

int DFRobot_VL53L4CD_I2C::getResult(VL53L4CD_Result_t *pResult)
{
  uint8_t block[24];
  uint16_t word;
  const uint16_t off_dist =
      (uint16_t)(VL53L4CD_REG_DISTANCE - VL53L4CD_REG_RANGE_STATUS);
  const uint16_t off_sig =
      (uint16_t)(VL53L4CD_REG_SIGNAL_RATE - VL53L4CD_REG_RANGE_STATUS);
  const uint16_t off_amb =
      (uint16_t)(VL53L4CD_REG_AMBIENT_RATE - VL53L4CD_REG_RANGE_STATUS);
  const uint16_t off_sigma =
      (uint16_t)(VL53L4CD_REG_SIGMA - VL53L4CD_REG_RANGE_STATUS);

  if (pResult == nullptr) {
    return 0;
  }
  if (!readResultBlock(block, sizeof(block))) {
    return 0;
  }

  pResult->range_status = mapRangeStatus(block[0]);

  word = (uint16_t)(((uint16_t)block[off_dist] << 8) | block[off_dist + 1u]);
  pResult->distance = word;

  word = (uint16_t)(((uint16_t)block[off_sig] << 8) | block[off_sig + 1u]);
  pResult->signal_rate_kcps = (uint32_t)word * 8u;
  word = (uint16_t)(((uint16_t)block[off_amb] << 8) | block[off_amb + 1u]);
  pResult->ambient_rate_kcps = (uint32_t)word * 8u;
  word = (uint16_t)(((uint16_t)block[off_sigma] << 8) | block[off_sigma + 1u]);
  pResult->sigma_mm = (uint16_t)(word / 4u);

  return 1;
}
