# DFRobot_VL53L4CD

Gravity VL53L4CD 测距：I2C 或 UART。Fermion 只使用 I2C。片内寄存器由 CS32 透出。

- Gravity I2C：把拨码拨到 I2C 和要使用的地址。
- Gravity UART：把拨码拨到 UART。9600 8N1，Modbus 从站 50。依赖 DFRobot_RTU。
- `setI2CAddress` 只在 Fermion 上生效，新地址存入 Flash。Gravity 调用不生效，返回 0，地址仍按拨码。不需要编译宏。

## Methods

```cpp
  /**
   * @brief 核对 MODEL_ID
   * @n CS32 固件上电已完成传感器初始化时，这里只读 0x010F，期望 0xEBAA
   * @return 1 型号正确，0 失败
   */
  int InitSensor(uint8_t addr = 0x52);

  /**
   * @brief 确认传感器已经可以测距
   * @n 固件上电已经完成初始化，这里只核对 MODEL_ID
   * @return 1 型号正确，0 失败
   */
  int sensorInit(void);

  /**
   * @brief 读取当前测量结果
   * @n 返回 1 只说明读到了结果区；range_status 为 0 才是有效距离
   * @return 1 读成功，0 总线失败
   */
  int getResult(VL53L4CD_Result_t *pResult);

  /**
   * @brief 用已知距离校准偏移
   * @n 目标物需稳定。先把偏移清零，再对 nb_samples 次有效测距取平均
   * @return 1 已写入新偏移，0 失败
   */
  int calibrateOffset(int16_t target_mm, int16_t *p_measured_offset_mm, int16_t nb_samples);
```

## 兼容性

| 主板 | 通过 | 未通过 | 未测试 |
| --- | :--: | :----: | :----: |
| Arduino UNO | √ |  |  |

## History

- 2026/10/09 - V1.0.0
