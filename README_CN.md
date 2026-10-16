# DFRobot_VL53L4CD

- [English Version](./README.md)

Gravity / Fermion VL53L4CD ToF 测距库。Gravity 支持 I2C 和 UART，Fermion 只支持 I2C。

## 产品

SKU：SEN0774（Gravity）/ SEN0775（Fermion）

## 目录

- [概述](#概述)
- [安装](#安装)
- [接口](#接口)
- [兼容性](#兼容性)
- [历史](#历史)
- [创作者](#创作者)

## 概述

测距范围最大 1200 mm。`range_status` 为 0 时，`distance` 才是有效距离，单位毫米。

- Gravity I2C：拨码拨到 I2C，地址拨到 0x52 或 0x53。
- Gravity UART：拨码拨到 UART。9600 8N1，Modbus 从站 50。依赖 DFRobot_RTU。
- Fermion：只有 I2C，出厂地址 0x52。`setI2CAddress` 可改地址并掉电保留。Gravity 调用该函数不生效。

模块上电后已经在连续测距。未接主机侧 XSHUT 时，模块保持使能。

## 安装

下载本库，放到 Arduino 的 `libraries` 目录，然后打开 `examples` 里的示例。

树莓派库在 `python/raspberrypi/`，说明见该目录下的 README。

## 接口

```cpp
  /**
   * @brief 核对 MODEL_ID
   * @n 上电后模块已开始测距，这里只读 0x010F，期望 0xEBAA
   * @n I2C：addr 为 7 位地址。Gravity 把地址拨码拨到对应位置
   * @n UART：忽略 addr，从站固定 50
   * @return 1 型号正确，0 失败
   */
  int InitSensor(uint8_t addr = 0x52);

  /**
   * @brief 确认传感器已经可以测距
   * @n 上电后模块已开始测距，这里只核对 MODEL_ID 为 0xEBAA
   * @return 1 型号正确，0 失败
   */
  int sensorInit(void);

  /**
   * @brief 读取本库版本，不是模块内部版本
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
   * @n 向 0x0087 写 0x21。模块上电后通常已在测距，需要重新开始时再调用
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
   * @n 返回 1 只说明读到了结果；range_status 为 0 才是有效距离
   * @return 1 读成功，0 总线失败
   */
  int getResult(VL53L4CD_Result_t *pResult);

  /**
   * @brief 设置测距偏移
   * @n 单位毫米，允许 -1024 到 1023
   * @return 1 成功，0 失败
   */
  int setOffset(int16_t offset_mm);

  /**
   * @brief 读取当前偏移，单位毫米
   * @return 1 成功，0 失败
   */
  int getOffset(int16_t *p_offset_mm);

  /**
   * @brief 用已知距离校准偏移
   * @n 目标物需稳定。先把偏移清零，再对 nb_samples 次有效测距取平均
   * @return 1 已写入新偏移，0 失败
   */
  int calibrateOffset(int16_t target_mm, int16_t *p_measured_offset_mm, int16_t nb_samples);

  /**
   * @brief 设置信号阈值，单位 kcps，最大 16384
   * @n 目标信号低于该值时，range_status 为 2
   * @return 1 成功，0 失败
   */
  int setSignalThreshold(uint16_t signal_kcps);

  /**
   * @brief 读取信号阈值，单位 kcps
   * @return 1 成功，0 失败
   */
  int getSignalThreshold(uint16_t *p_signal_kcps);

  /**
   * @brief 设置 sigma 阈值，单位毫米，最大 16383
   * @n 实测 sigma 高于该值时，range_status 为 1
   * @return 1 成功，0 失败
   */
  int setSigmaThreshold(uint16_t sigma_mm);

  /**
   * @brief 读取 sigma 阈值，单位毫米
   * @return 1 成功，0 失败
   */
  int getSigmaThreshold(uint16_t *p_sigma_mm);

  /**
   * @brief 设置 GPIO1 距离窗口
   * @n window=0：低于 low_mm
   * @n window=1：高于 high_mm
   * @n window=2：低于 low_mm 或高于 high_mm
   * @n window=3：处于 low_mm 与 high_mm 之间
   * @return 1 成功，0 失败
   */
  int setDetectionThresholds(uint16_t low_mm, uint16_t high_mm, uint8_t window);

  /**
   * @brief 读取 GPIO1 距离窗口
   * @return 1 成功，0 失败
   */
  int getDetectionThresholds(uint16_t *p_low_mm, uint16_t *p_high_mm, uint8_t *p_window);

  /**
   * @brief 修改传感器 I2C 地址
   * @n 只有 Fermion 会改，并掉电保留。Gravity 用拨码选地址，调用不生效
   * @return 1 已修改，0 拒绝或失败
   */
  virtual int setI2CAddress(uint8_t addr);

  /**
   * @brief I2C 构造。xshut_pin 为 -1 时不操作引脚，模块默认保持使能
   */
  DFRobot_VL53L4CD_I2C(TwoWire *i2c = &Wire, int xshut_pin = -1);
  void powerOn(void);
  void powerOff(void);
  int begin(void);

  /**
   * @brief Gravity UART 构造。Fermion 没有 UART，不要用本类
   * @n 模块保持 9600 8N1，从站 50。调用 begin() 前先把串口按 9600 打开
   */
  explicit DFRobot_VL53L4CD_UART(Stream *serial, uint32_t baudRate = 9600);
  int begin(void);
```

## 兼容性

| 主板 | 通过 | 未通过 | 未测试 | 备注 |
| --- | :--: | :----: | :----: | --- |
| Arduino UNO | √ |  |  |  |
| Arduino Mega2560 | √ |  |  |  |
| Arduino Leonardo |  |  | √ |  |
| Raspberry Pi |  |  | √ | 仅 I2C，见 python/raspberrypi |

## 历史

- 2026/10/09 - V1.0.0

## 创作者

Written by wxzed(xiao.wu@dfrobot.com), 2026. (Welcome to our [website](https://www.dfrobot.com/))
