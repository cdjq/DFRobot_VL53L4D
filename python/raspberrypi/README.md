# DFRobot_VL53L4CD

Raspberry Pi I2C library for Gravity and Fermion VL53L4CD. UART is not implemented. The onboard CS32 already initializes the sensor and starts ranging.

- Gravity: set the DIP switches to I2C and match the constructor address (0x52 or 0x53).
- Fermion: ships at 0x52. `set_i2c_address` changes the address and keeps it after power off. Gravity ignores it.
- Wiring: SDA = GPIO2, SCL = GPIO3. Use `xshut_pin=-1` when the host does not drive XSHUT.

## Contents

```
python/raspberrypi/
├── DFRobot_VL53L4CD.py
├── README.md
├── README_CN.md
└── examples/
```

## Install

```
sudo apt-get install python3-smbus python3-smbus2 python3-rpi.gpio
```

Keep `sys.path.append("../../")` in the examples, or place this directory on `PYTHONPATH`.

## Methods

| Python | Description |
| --- | --- |
| `begin()` | Open I2C. Drives XSHUT high when a pin was given. |
| `power_on()` / `power_off()` | Host-side XSHUT. No GPIO action when the pin is -1. |
| `init_sensor(addr=0x52)` | Store the address and check MODEL_ID 0xEBAA. |
| `sensor_init()` | Check MODEL_ID. Does not rewrite ULD defaults. |
| `get_version()` | Return the library version object. |
| `get_sensor_id()` | Return the device id, or None. |
| `start_ranging()` / `stop_ranging()` | Write 0x0087. Firmware already started ranging. |
| `get_result()` | Return a result object. `distance` is valid when `range_status == 0`. |
| `set_offset(offset_mm)` | Offset from -1024 to 1023 mm. |
| `get_offset()` | Return the current offset, or None. |
| `calibrate_offset(target_mm, nb_samples)` | Return the stored offset, or None. |
| `set_signal_threshold(signal_kcps)` | Signal threshold, maximum 16384. |
| `get_signal_threshold()` | Return the signal threshold. |
| `set_sigma_threshold(sigma_mm)` | Sigma threshold, maximum 16383. |
| `get_sigma_threshold()` | Return the sigma threshold. |
| `set_detection_thresholds(low_mm, high_mm, window)` | GPIO1 window. `window` is 0..3. |
| `get_detection_thresholds()` | Return `(low_mm, high_mm, window)`. |
| `set_i2c_address(addr)` | Fermion changes the 7-bit address. Gravity ignores it and returns False. |

## Compatibility

| Board | Python | Work Well | Work Wrong | Untested |
| --- | --- | :--: | :----: | :----: |
| Raspberry Pi 4 | Python3 |  |  | √ |
| Raspberry Pi 3 | Python3 |  |  | √ |
| Raspberry Pi 2 | Python3 |  |  | √ |

## History

- 2026/10/15 - V1.0.0

## Creator

- wxzed
