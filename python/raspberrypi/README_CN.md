# DFRobot_VL53L4CD

树莓派通过 I2C 读取 Gravity / Fermion VL53L4CD。本库不实现 UART。模块上的 CS32 已经完成传感器初始化和连续测距。

- Gravity：拨码拨到 I2C，地址与构造参数一致（0x52 或 0x53）。
- Fermion：出厂地址 0x52。`set_i2c_address` 改地址，掉电保留。Gravity 调用不生效。
- 接线：SDA = GPIO2，SCL = GPIO3。未接主机侧 XSHUT 时 `xshut_pin=-1`。

## 目录

```
python/raspberrypi/
├── DFRobot_VL53L4CD.py
├── README.md
├── README_CN.md
└── examples/
```

## 库安装

```
sudo apt-get install python3-smbus python3-smbus2 python3-rpi.gpio
```

把本目录放到脚本可导入的路径，或在示例里保留 `sys.path.append("../../")`。

## 方法

| Python | 说明 |
| --- | --- |
| `begin()` | 打开 I2C。给了 XSHUT 引脚时先拉高。 |
| `power_on()` / `power_off()` | 主机侧 XSHUT。未接线时不操作 GPIO。 |
| `init_sensor(addr=0x52)` | 记录地址并核对 MODEL_ID 0xEBAA。 |
| `sensor_init()` | 只核对 MODEL_ID，不重写 ULD 默认参数。 |
| `get_version()` | 返回库版本对象。 |
| `get_sensor_id()` | 返回器件 ID，失败为 None。 |
| `start_ranging()` / `stop_ranging()` | 写 0x0087。固件上电已开始测距。 |
| `get_result()` | 返回结果对象。`range_status == 0` 时 `distance` 有效。 |
| `set_offset(offset_mm)` | 偏移 -1024..1023 mm。 |
| `get_offset()` | 返回当前偏移，失败为 None。 |
| `calibrate_offset(target_mm, nb_samples)` | 返回写入的偏移，失败为 None。 |
| `set_signal_threshold(signal_kcps)` | 信号阈值，最大 16384。 |
| `get_signal_threshold()` | 返回信号阈值。 |
| `set_sigma_threshold(sigma_mm)` | sigma 阈值，最大 16383。 |
| `get_sigma_threshold()` | 返回 sigma 阈值。 |
| `set_detection_thresholds(low_mm, high_mm, window)` | GPIO1 窗口，window 为 0..3。 |
| `get_detection_thresholds()` | 返回 `(low_mm, high_mm, window)`。 |
| `set_i2c_address(addr)` | Fermion 修改 7 位地址。Gravity 不生效，返回 False。 |

## 兼容性

| 主板 | Python | 通过 | 未通过 | 未测试 |
| --- | --- | :--: | :----: | :----: |
| Raspberry Pi 4 | Python3 |  |  | √ |
| Raspberry Pi 3 | Python3 |  |  | √ |
| Raspberry Pi 2 | Python3 |  |  | √ |

## 历史

- 2026/10/15 - V1.0.0

## 创作者

- wxzed
