/*!
 * @file DFRobot_VL53L4CD.h
 * @brief VL53L4CD ToF sensor (Gravity I2C/UART, Fermion I2C)
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-09
 * @url https://github.com/cdjq/DFRobot_VL53L4D
 */
#ifndef __DFROBOT_VL53L4CD_H__
#define __DFROBOT_VL53L4CD_H__

#include <Arduino.h>
#include <Wire.h>

#define VL53L4CD_MODEL_ID 0xEBAAu ///< VL53L4CD 器件 ID

#define VL53L4CD_REG_MODEL_ID 0x010Fu       ///< IDENTIFICATION__MODEL_ID
#define VL53L4CD_REG_I2C_ADDR 0x0001u       ///< 模块对外 I2C 地址，值为 7 位地址
#define VL53L4CD_REG_OFFSET 0x001Eu         ///< RANGE_OFFSET_MM，单位为 0.25 mm
#define VL53L4CD_REG_INNER_OFFSET 0x0020u   ///< INNER_OFFSET_MM
#define VL53L4CD_REG_OUTER_OFFSET 0x0022u   ///< OUTER_OFFSET_MM
#define VL53L4CD_REG_INTERRUPT 0x0046u      ///< SYSTEM__INTERRUPT，距离窗口 0..3
#define VL53L4CD_REG_SIGMA_TH 0x0064u       ///< RANGE_CONFIG__SIGMA_THRESH
#define VL53L4CD_REG_SIGNAL_TH 0x0066u      ///< MIN_COUNT_RATE_RTN_LIMIT_MCPS
#define VL53L4CD_REG_THRESH_HIGH 0x0072u    ///< THRESH_HIGH
#define VL53L4CD_REG_THRESH_LOW 0x0074u     ///< THRESH_LOW
#define VL53L4CD_REG_RANGE_STATUS 0x0089u   ///< RESULT__RANGE_STATUS
#define VL53L4CD_REG_SPAD_NB 0x008Cu        ///< RESULT__SPAD_NB
#define VL53L4CD_REG_SIGNAL_RATE 0x008Eu    ///< RESULT__SIGNAL_RATE
#define VL53L4CD_REG_AMBIENT_RATE 0x0090u   ///< RESULT__AMBIENT_RATE
#define VL53L4CD_REG_SIGMA 0x0092u          ///< RESULT__SIGMA
#define VL53L4CD_REG_DISTANCE 0x0096u       ///< RESULT__DISTANCE
#define VL53L4CD_REG_SYSTEM_START 0x0087u   ///< SYSTEM_START
#define VL53L4CD_REG_INT_CLEAR 0x0086u      ///< SYSTEM__INTERRUPT_CLEAR

#define VL53L4CD_MAX_DISTANCE_MM 1200u      ///< 规格最大测距（mm）
#define VL53L4CD_RTU_ADDR 50u               ///< Gravity UART 默认 Modbus 从站

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
  uint16_t number_of_spad;
  uint32_t ambient_rate_kcps;
  uint32_t signal_rate_kcps;
  uint32_t ambient_per_spad_kcps;
  uint32_t signal_per_spad_kcps;
  uint16_t sigma_mm;
} VL53L4CD_Result_t;

/**
 * @class DFRobot_VL53L4CD
 * @brief I2C 与 UART 共用的测距 API。寄存器换算与 ST VL53L4CD ULD 一致。
 */
class DFRobot_VL53L4CD {
public:
  /**
   * @brief 核对 MODEL_ID
   * @n CS32 固件上电已完成传感器初始化时，这里只读 0x010F，期望 0xEBAA
   * @n I2C：addr 为 7 位地址。Gravity 把地址拨码拨到对应位置
   * @n UART：忽略 addr，从站固定 50
   * @return 1 型号正确，0 失败
   */
  int InitSensor(uint8_t addr = 0x52);

  /**
   * @brief 确认传感器已经可以测距
   * @n 固件上电已经完成 SensorInit 和 StartRanging，这里只核对 MODEL_ID 为 0xEBAA
   * @n 不再从主机重写一遍 ULD 默认参数
   * @return 1 型号正确，0 失败
   */
  int sensorInit(void);

  /**
   * @brief 读取本库版本，不是传感器固件版本
   * @n major.minor.build.revision 对应当前库 V1.0.0
   * @return 1 成功，0 空指针
   */
  int getVersion(VL53L4CD_Version_t *pVersion);

  /**
   * @brief 读取器件 ID
   * @n 寄存器 0x010F，VL53L4CD 应为 0xEBAA
   * @return 1 成功，0 失败
   */
  int getSensorId(uint16_t *p_id);

  /**
   * @brief 开始连续测距
   * @n 向 0x0087 写 0x21。固件上电已经开始测距时可以不调用
   * @return 1 成功，0 总线失败
   */
  int startRanging(void);

  /**
   * @brief 停止测距
   * @n 向 0x0087 写 0x80
   * @return 1 成功，0 总线失败
   */
  int stopRanging(void);

  /**
   * @brief 读取当前测量结果
   * @n 返回 1 只说明读到了结果区；range_status 为 0 才是有效距离
   * @n 非 0 表示超量程、无目标或阈值未满足，distance 不要当成有效毫米
   * @return 1 读成功，0 总线失败
   */
  int getResult(VL53L4CD_Result_t *pResult);

  /**
   * @brief 设置测距偏移
   * @n 单位毫米，允许 -1024 到 1023。超出范围不写寄存器并返回 0
   * @n 写入 0x001E（数值乘 4），并清零 0x0020 与 0x0022
   * @return 1 成功，0 失败
   */
  int setOffset(int16_t offset_mm);

  /**
   * @brief 读取当前偏移
   * @n 单位毫米，来自 0x001E
   * @return 1 成功，0 失败
   */
  int getOffset(int16_t *p_offset_mm);

  /**
   * @brief 用已知距离校准偏移
   * @n 目标物需稳定，距离单位毫米。先把偏移清零，再对 nb_samples 次有效测距取平均
   * @n 新偏移 = 目标距离 - 平均实测，并写入 p_measured_offset_mm。没有一次有效测距则返回 0
   * @return 1 已写入新偏移，0 失败
   */
  int calibrateOffset(int16_t target_mm, int16_t *p_measured_offset_mm, int16_t nb_samples);

  /**
   * @brief 设置信号阈值
   * @n 单位 kcps。0 表示不设阈值。默认约 1024。最大 16384，超过返回 0
   * @n 目标信号低于该值时，range_status 为 2
   * @return 1 成功，0 失败
   */
  int setSignalThreshold(uint16_t signal_kcps);

  /**
   * @brief 读取信号阈值
   * @n 单位 kcps
   * @return 1 成功，0 失败
   */
  int getSignalThreshold(uint16_t *p_signal_kcps);

  /**
   * @brief 设置 sigma 阈值
   * @n 单位毫米。0 表示不设阈值。默认约 15。最大 16383，超过返回 0
   * @n 实测 sigma 高于该值时，range_status 为 1
   * @return 1 成功，0 失败
   */
  int setSigmaThreshold(uint16_t sigma_mm);

  /**
   * @brief 读取 sigma 阈值
   * @n 单位毫米
   * @return 1 成功，0 失败
   */
  int getSigmaThreshold(uint16_t *p_sigma_mm);

  /**
   * @brief 设置 GPIO1 距离窗口
   * @n window=0：低于 low_mm
   * @n window=1：高于 high_mm
   * @n window=2：低于 low_mm 或高于 high_mm
   * @n window=3：处于 low_mm 与 high_mm 之间
   * @n 其它 window 返回 0。模块 GPIO1 焊盘输出，库不接这根脚
   * @return 1 成功，0 失败
   */
  int setDetectionThresholds(uint16_t low_mm, uint16_t high_mm, uint8_t window);

  /**
   * @brief 读取 GPIO1 距离窗口
   * @n window 含义与 setDetectionThresholds 相同，只保留低 3 位
   * @return 1 成功，0 失败
   */
  int getDetectionThresholds(uint16_t *p_low_mm, uint16_t *p_high_mm, uint8_t *p_window);

  /**
   * @brief 修改传感器 I2C 地址
   * @n 只有 Fermion 会改。Gravity 用拨码选地址，调用不生效
   * @n 基类和 UART 版直接返回 0
   * @return 1 已修改，0 拒绝或失败
   */
  virtual int setI2CAddress(uint8_t addr);

protected:
  virtual void onInitAddress(uint8_t addr);
  virtual bool writeReg8(uint16_t reg, uint8_t value) = 0;
  virtual bool writeReg16(uint16_t reg, uint16_t value) = 0;
  virtual bool readReg8(uint16_t reg, uint8_t *value) = 0;
  virtual bool readReg16(uint16_t reg, uint16_t *value) = 0;
  virtual bool readResultBlock(uint8_t *buf, uint8_t len) = 0;
  static uint8_t mapRangeStatus(uint8_t raw);
};

/**
 * @class DFRobot_VL53L4CD_I2C
 * @brief Gravity / Fermion I2C。寄存器指针 16 位，大端。
 */
class DFRobot_VL53L4CD_I2C : public DFRobot_VL53L4CD {
public:
  /**
   * @brief 绑定 I2C 和可选的主机侧 XSHUT
   * @n xshut_pin 为 -1 时不操作引脚，模块板载 XSHUT 由 CS32 拉高
   * @n 传入引脚时，powerOn 拉高、powerOff 拉低
   */
  DFRobot_VL53L4CD_I2C(TwoWire *i2c = &Wire, int xshut_pin = -1);

  /**
   * @brief 拉高主机侧 XSHUT
   * @n 未传入引脚时不做任何事
   */
  void powerOn(void);

  /**
   * @brief 拉低主机侧 XSHUT
   * @n 未传入引脚时不做任何事
   */
  void powerOff(void);

  /**
   * @brief 打开 I2C 主机
   * @n 若构造时给了 XSHUT，先拉高再 Wire.begin
   * @return 1 成功，0 未绑定总线
   */
  int begin(void);

  /**
   * @brief 修改传感器 I2C 地址
   * @n 把 7 位地址原样写入 0x0001，约 50 ms 后用新地址读回 0x0001 确认
   * @n Fermion 改地址并存入 Flash，掉电保留。Gravity 固件拒绝，读回不一致时返回 0，仍用原地址
   * @n 新地址和当前地址相同时，读回一致也返回 1
   * @param addr 7 位地址，0x08 到 0x77
   * @return 1 已修改，0 拒绝或失败
   */
  int setI2CAddress(uint8_t addr);

protected:
  bool writeReg8(uint16_t reg, uint8_t value);
  bool writeReg16(uint16_t reg, uint16_t value);
  bool readReg8(uint16_t reg, uint8_t *value);
  bool readReg16(uint16_t reg, uint16_t *value);
  bool readResultBlock(uint8_t *buf, uint8_t len);
  void onInitAddress(uint8_t addr);

private:
  TwoWire *_i2c;
  int _xshut;
  uint8_t _addr;
};

/**
 * @class DFRobot_VL53L4CD_UART
 * @brief Gravity UART。Fermion 没有 UART，不要用本类。
 * @n 模块固定 9600 8N1，从站 50。构造里的波特率参数只为兼容，不会改模块。
 */
class DFRobot_VL53L4CD_UART : public DFRobot_VL53L4CD {
public:
  /**
   * @brief 绑定已经 begin(9600) 的串口
   * @n 仅 Gravity。波特率参数不切换模块，模块保持 9600 8N1
   * @n 调用 begin() 前先让 serial 以 9600 启动
   */
  explicit DFRobot_VL53L4CD_UART(Stream *serial, uint32_t baudRate = 9600);

  /**
   * @brief 释放 RTU 对象
   * @n 不关闭已经打开的串口
   */
  ~DFRobot_VL53L4CD_UART();

  /**
   * @brief 准备 RTU 主机
   * @n 不改模块波特率。串口需已按 9600 8N1 打开
   * @return 1 成功，0 未绑定串口
   */
  int begin(void);

protected:
  bool writeReg8(uint16_t reg, uint8_t value);
  bool writeReg16(uint16_t reg, uint16_t value);
  bool readReg8(uint16_t reg, uint8_t *value);
  bool readReg16(uint16_t reg, uint16_t *value);
  bool readResultBlock(uint8_t *buf, uint8_t len);

private:
  Stream *_serial;
  uint32_t _baud;
  void *_rtu;
};

#endif
