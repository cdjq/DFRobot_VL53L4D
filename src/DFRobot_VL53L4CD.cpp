/*!
 * @file DFRobot_VL53L4CD.cpp
 * @brief VL53L4CD I2C / UART 实现。寄存器换算与 ST ULD 一致。
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-09
 */
#include "DFRobot_VL53L4CD.h"
#include <DFRobot_RTU.h>

#define LIB_VER_MAJOR 1u
#define LIB_VER_MINOR 0u
#define LIB_VER_BUILD 0u
#define LIB_VER_REV 0u

uint8_t DFRobot_VL53L4CD::mapRangeStatus(uint8_t raw)
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

void DFRobot_VL53L4CD::onInitAddress(uint8_t addr)
{
  (void)addr;
}

int DFRobot_VL53L4CD::sensorInit(void)
{
  uint16_t id = 0;

  if (!getSensorId(&id) || id != VL53L4CD_MODEL_ID) {
    return 0;
  }
  return 1;
}

int DFRobot_VL53L4CD::InitSensor(uint8_t addr)
{
  onInitAddress(addr);
  return sensorInit();
}

int DFRobot_VL53L4CD::getVersion(VL53L4CD_Version_t *pVersion)
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

int DFRobot_VL53L4CD::getSensorId(uint16_t *p_id)
{
  if (p_id == nullptr || !readReg16(VL53L4CD_REG_MODEL_ID, p_id)) {
    return 0;
  }
  return 1;
}

int DFRobot_VL53L4CD::startRanging(void)
{
  if (!writeReg8(VL53L4CD_REG_SYSTEM_START, 0x21u)) {
    return 0;
  }
  return 1;
}

int DFRobot_VL53L4CD::stopRanging(void)
{
  if (!writeReg8(VL53L4CD_REG_SYSTEM_START, 0x80u)) {
    return 0;
  }
  return 1;
}

int DFRobot_VL53L4CD::getResult(VL53L4CD_Result_t *pResult)
{
  uint8_t block[24];
  uint16_t word;
  uint16_t raw_spads;
  const uint16_t off_dist =
      (uint16_t)(VL53L4CD_REG_DISTANCE - VL53L4CD_REG_RANGE_STATUS);
  const uint16_t off_sig =
      (uint16_t)(VL53L4CD_REG_SIGNAL_RATE - VL53L4CD_REG_RANGE_STATUS);
  const uint16_t off_amb =
      (uint16_t)(VL53L4CD_REG_AMBIENT_RATE - VL53L4CD_REG_RANGE_STATUS);
  const uint16_t off_sigma =
      (uint16_t)(VL53L4CD_REG_SIGMA - VL53L4CD_REG_RANGE_STATUS);
  const uint16_t off_spad =
      (uint16_t)(VL53L4CD_REG_SPAD_NB - VL53L4CD_REG_RANGE_STATUS);

  if (pResult == nullptr) {
    return 0;
  }
  if (!readResultBlock(block, sizeof(block))) {
    return 0;
  }

  pResult->range_status = mapRangeStatus(block[0]);
  raw_spads = (uint16_t)(((uint16_t)block[off_spad] << 8) | block[off_spad + 1u]);
  pResult->number_of_spad = (uint16_t)(raw_spads / 256u);

  word = (uint16_t)(((uint16_t)block[off_dist] << 8) | block[off_dist + 1u]);
  pResult->distance = word;

  word = (uint16_t)(((uint16_t)block[off_sig] << 8) | block[off_sig + 1u]);
  pResult->signal_rate_kcps = (uint32_t)word * 8u;
  word = (uint16_t)(((uint16_t)block[off_amb] << 8) | block[off_amb + 1u]);
  pResult->ambient_rate_kcps = (uint32_t)word * 8u;
  word = (uint16_t)(((uint16_t)block[off_sigma] << 8) | block[off_sigma + 1u]);
  pResult->sigma_mm = (uint16_t)(word / 4u);

  if (raw_spads == 0u) {
    pResult->signal_per_spad_kcps = 0;
    pResult->ambient_per_spad_kcps = 0;
  } else {
    pResult->signal_per_spad_kcps = pResult->signal_rate_kcps * 256u / raw_spads;
    pResult->ambient_per_spad_kcps = pResult->ambient_rate_kcps * 256u / raw_spads;
  }
  return 1;
}

int DFRobot_VL53L4CD::setOffset(int16_t offset_mm)
{
  uint16_t temp;

  if (offset_mm < -1024 || offset_mm > 1023) {
    return 0;
  }
  temp = (uint16_t)((uint16_t)offset_mm * (uint16_t)4u);
  if (!writeReg16(VL53L4CD_REG_OFFSET, temp)) {
    return 0;
  }
  if (!writeReg16(VL53L4CD_REG_INNER_OFFSET, 0)) {
    return 0;
  }
  if (!writeReg16(VL53L4CD_REG_OUTER_OFFSET, 0)) {
    return 0;
  }
  return 1;
}

int DFRobot_VL53L4CD::getOffset(int16_t *p_offset_mm)
{
  uint16_t temp = 0;

  if (p_offset_mm == nullptr || !readReg16(VL53L4CD_REG_OFFSET, &temp)) {
    return 0;
  }
  temp = (uint16_t)(temp << 3);
  temp = (uint16_t)(temp >> 5);
  *p_offset_mm = (int16_t)temp;
  if (*p_offset_mm > 1024) {
    *p_offset_mm = (int16_t)(*p_offset_mm - 2048);
  }
  return 1;
}

int DFRobot_VL53L4CD::calibrateOffset(int16_t target_mm, int16_t *p_measured_offset_mm,
                                    int16_t nb_samples)
{
  int32_t sum = 0;
  int16_t got = 0;
  int16_t i;
  int16_t offset;

  if (p_measured_offset_mm == nullptr || nb_samples <= 0 || setOffset(0) == 0) {
    return 0;
  }
  for (i = 0; i < nb_samples; i++) {
    VL53L4CD_Result_t result;

    delay(50);
    if (getResult(&result) == 0 || result.range_status != 0u) {
      continue;
    }
    sum += (int32_t)result.distance;
    got++;
  }
  if (got == 0) {
    return 0;
  }
  offset = (int16_t)(target_mm - (int16_t)(sum / got));
  if (setOffset(offset) == 0 || getOffset(p_measured_offset_mm) == 0) {
    return 0;
  }
  return 1;
}

int DFRobot_VL53L4CD::setSignalThreshold(uint16_t signal_kcps)
{
  if (signal_kcps > 16384u) {
    return 0;
  }
  if (!writeReg16(VL53L4CD_REG_SIGNAL_TH, (uint16_t)(signal_kcps >> 3))) {
    return 0;
  }
  return 1;
}

int DFRobot_VL53L4CD::getSignalThreshold(uint16_t *p_signal_kcps)
{
  uint16_t temp = 0;

  if (p_signal_kcps == nullptr || !readReg16(VL53L4CD_REG_SIGNAL_TH, &temp)) {
    return 0;
  }
  *p_signal_kcps = (uint16_t)(temp << 3);
  return 1;
}

int DFRobot_VL53L4CD::setSigmaThreshold(uint16_t sigma_mm)
{
  if (sigma_mm > (uint16_t)(0xFFFFu >> 2)) {
    return 0;
  }
  if (!writeReg16(VL53L4CD_REG_SIGMA_TH, (uint16_t)(sigma_mm << 2))) {
    return 0;
  }
  return 1;
}

int DFRobot_VL53L4CD::getSigmaThreshold(uint16_t *p_sigma_mm)
{
  uint16_t temp = 0;

  if (p_sigma_mm == nullptr || !readReg16(VL53L4CD_REG_SIGMA_TH, &temp)) {
    return 0;
  }
  *p_sigma_mm = (uint16_t)(temp >> 2);
  return 1;
}

int DFRobot_VL53L4CD::setDetectionThresholds(uint16_t low_mm, uint16_t high_mm, uint8_t window)
{
  if (window > 3u) {
    return 0;
  }
  if (!writeReg8(VL53L4CD_REG_INTERRUPT, window)) {
    return 0;
  }
  if (!writeReg16(VL53L4CD_REG_THRESH_HIGH, high_mm)) {
    return 0;
  }
  if (!writeReg16(VL53L4CD_REG_THRESH_LOW, low_mm)) {
    return 0;
  }
  return 1;
}

int DFRobot_VL53L4CD::getDetectionThresholds(uint16_t *p_low_mm, uint16_t *p_high_mm,
                                            uint8_t *p_window)
{
  uint8_t window = 0;

  if (p_low_mm == nullptr || p_high_mm == nullptr || p_window == nullptr) {
    return 0;
  }
  if (!readReg16(VL53L4CD_REG_THRESH_HIGH, p_high_mm)) {
    return 0;
  }
  if (!readReg16(VL53L4CD_REG_THRESH_LOW, p_low_mm)) {
    return 0;
  }
  if (!readReg8(VL53L4CD_REG_INTERRUPT, &window)) {
    return 0;
  }
  *p_window = (uint8_t)(window & 0x7u);
  return 1;
}

int DFRobot_VL53L4CD::setI2CAddress(uint8_t addr)
{
  (void)addr;
  return 0;
}

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

void DFRobot_VL53L4CD_I2C::onInitAddress(uint8_t addr)
{
  _addr = addr;
}

int DFRobot_VL53L4CD_I2C::setI2CAddress(uint8_t addr)
{
  uint8_t old_addr = _addr;
  uint8_t readback = 0;

  if (addr < 0x08u || addr > 0x77u || !writeReg8(VL53L4CD_REG_I2C_ADDR, addr)) {
    return 0;
  }
  /* 模块先应答这笔写，再在主循环里改地址并写 Flash。Gravity 会在那一步拒绝，
     所以只看写成功不够，要等它处理完再用新地址读回。 */
  delay(50);
  _addr = addr;
  if (readReg8(VL53L4CD_REG_I2C_ADDR, &readback) && readback == addr) {
    return 1;
  }
  _addr = old_addr;
  return 0;
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

DFRobot_VL53L4CD_UART::DFRobot_VL53L4CD_UART(Stream *serial, uint32_t baudRate)
    : _serial(serial), _baud(baudRate), _rtu(nullptr)
{
  if (_serial != nullptr) {
    _rtu = new DFRobot_RTU(_serial);
  }
}

DFRobot_VL53L4CD_UART::~DFRobot_VL53L4CD_UART()
{
  delete static_cast<DFRobot_RTU *>(_rtu);
  _rtu = nullptr;
}

int DFRobot_VL53L4CD_UART::begin(void)
{
  DFRobot_RTU *rtu = static_cast<DFRobot_RTU *>(_rtu);

  if (_serial == nullptr || rtu == nullptr) {
    return 0;
  }
  (void)_baud;
  rtu->setTimeoutTimeMs(80);
  return 1;
}

bool DFRobot_VL53L4CD_UART::writeReg16(uint16_t reg, uint16_t value)
{
  DFRobot_RTU *rtu = static_cast<DFRobot_RTU *>(_rtu);

  if (rtu == nullptr) {
    return false;
  }
  return rtu->writeHoldingRegister(VL53L4CD_RTU_ADDR, reg, value) == 0;
}

bool DFRobot_VL53L4CD_UART::writeReg8(uint16_t reg, uint8_t value)
{
  /* 模块按 16 位大端写入，高字节落在该寄存器地址上。 */
  return writeReg16(reg, (uint16_t)((uint16_t)value << 8));
}

bool DFRobot_VL53L4CD_UART::readReg16(uint16_t reg, uint16_t *value)
{
  DFRobot_RTU *rtu = static_cast<DFRobot_RTU *>(_rtu);

  if (rtu == nullptr || value == nullptr) {
    return false;
  }
  return rtu->readHoldingRegister(VL53L4CD_RTU_ADDR, reg, value, 1) == 0;
}

bool DFRobot_VL53L4CD_UART::readReg8(uint16_t reg, uint8_t *value)
{
  uint16_t word = 0;

  if (value == nullptr || !readReg16(reg, &word)) {
    return false;
  }
  *value = (uint8_t)(word >> 8);
  return true;
}

bool DFRobot_VL53L4CD_UART::readResultBlock(uint8_t *buf, uint8_t len)
{
  const uint16_t regs[] = {
      VL53L4CD_REG_RANGE_STATUS, VL53L4CD_REG_SPAD_NB, VL53L4CD_REG_SIGNAL_RATE,
      VL53L4CD_REG_AMBIENT_RATE, VL53L4CD_REG_SIGMA, VL53L4CD_REG_DISTANCE};

  if (buf == nullptr || len < 24u) {
    return false;
  }
  for (uint8_t i = 0; i < len; i++) {
    buf[i] = 0;
  }
  for (uint8_t i = 0; i < (uint8_t)(sizeof(regs) / sizeof(regs[0])); i++) {
    uint16_t word = 0;
    uint16_t off = (uint16_t)(regs[i] - VL53L4CD_REG_RANGE_STATUS);

    if (off + 1u >= len || !readReg16(regs[i], &word)) {
      return false;
    }
    buf[off] = (uint8_t)(word >> 8);
    buf[off + 1u] = (uint8_t)(word & 0xFFu);
  }
  return true;
}
