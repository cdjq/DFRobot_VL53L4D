/*!
 * @file DFRobot_VL53L4CD.h
 * @brief VL53L4CD ToF sensor (Gravity I2C/UART, Fermion I2C)
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-09
 * @url https://github.com/DFRobot/DFRobot_VL53L4CD
 */
#ifndef __DFROBOT_VL53L4CD_H__
#define __DFROBOT_VL53L4CD_H__

#include <Arduino.h>
#include <Wire.h>

#define VL53L4CD_MODEL_ID 0xEBAAu ///< VL53L4CD 器件 ID

#define VL53L4CD_REG_MODEL_ID 0x010Fu       ///< IDENTIFICATION__MODEL_ID
#define VL53L4CD_REG_RANGE_STATUS 0x0089u   ///< RESULT__RANGE_STATUS
#define VL53L4CD_REG_SIGNAL_RATE 0x008Eu    ///< RESULT__SIGNAL_RATE（32 位）
#define VL53L4CD_REG_AMBIENT_RATE 0x0090u   ///< RESULT__AMBIENT_RATE（32 位）
#define VL53L4CD_REG_SIGMA 0x0092u          ///< RESULT__SIGMA
#define VL53L4CD_REG_DISTANCE 0x0096u       ///< RESULT__DISTANCE
#define VL53L4CD_REG_SYSTEM_START 0x0087u   ///< SYSTEM_START
#define VL53L4CD_REG_INT_CLEAR 0x0086u      ///< SYSTEM__INTERRUPT_CLEAR

#define VL53L4CD_MAX_DISTANCE_MM 1200u      ///< 规格最大测距（mm）

/**
 * @struct VL53L4CD_Version_t
 * @brief 库版本
 */
typedef struct {
  uint8_t major;
  uint8_t minor;
  uint8_t build;
  uint8_t revision;
} VL53L4CD_Version_t;

/**
 * @struct VL53L4CD_Result_t
 * @brief 单次测距结果
 */
typedef struct {
  uint8_t range_status;
  uint16_t distance;
  uint32_t ambient_rate_kcps;
  uint32_t signal_rate_kcps;
  uint16_t sigma_mm;
} VL53L4CD_Result_t;

/**
 * @class DFRobot_VL53L4CD_I2C
 * @brief Gravity / Fermion I2C 模式（经 CS32 寄存器透传）
 */
class DFRobot_VL53L4CD_I2C {
public:
  /**
   * @param i2c TwoWire 实例，默认 &Wire
   * @param xshut_pin 主机侧 XSHUT，模块由 CS32 控制时传 -1
   */
  DFRobot_VL53L4CD_I2C(TwoWire *i2c = &Wire, int xshut_pin = -1);

  void powerOn(void);
  void powerOff(void);
  int begin(void);
  /**
   * @brief 校验 MODEL_ID；CS32 固件已初始化时仅读 ID
   * @param addr 模块 I2C 7 位地址，Gravity 默认 0x52，拨码 0x53
   * @n Gravity 用拨码选 0x52/0x53，勿调用 setI2CAddress
   */
  int InitSensor(uint8_t addr = 0x52);
  int getVersion(VL53L4CD_Version_t *pVersion);
  int getSensorId(uint16_t *p_id);
  int startRanging(void);
  int stopRanging(void);
  /**
   * @brief 读取当前测量结果块
   * @return 1=I2C 读成功；0=总线失败
   * @n 返回 1 时请用 range_status 判断有效性：0 表示有效测距，非 0 表示超量程/无目标等
   */
  int getResult(VL53L4CD_Result_t *pResult);

protected:
  bool writeReg8(uint16_t reg, uint8_t value);
  bool writeReg16(uint16_t reg, uint16_t value);
  bool readReg8(uint16_t reg, uint8_t *value);
  bool readReg16(uint16_t reg, uint16_t *value);
  bool readReg32(uint16_t reg, uint32_t *value);
  bool readResultBlock(uint8_t *buf, uint8_t len);
  static uint8_t mapRangeStatus(uint8_t raw);

private:
  TwoWire *_i2c;
  int _xshut;
  uint8_t _addr;
};

#endif
