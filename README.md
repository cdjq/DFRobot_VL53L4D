# DFRobot_VL53L4CD

- [中文版](./README_CN.md)

ToF distance library for the Gravity and Fermion VL53L4CD. Gravity supports I2C and UART. Fermion supports I2C only.

## Product

SKU: SEN0774 (Gravity) / SEN0775 (Fermion)

## Table of Contents

- [Summary](#summary)
- [Installation](#installation)
- [Methods](#methods)
- [Compatibility](#compatibility)
- [History](#history)
- [Credits](#credits)

## Summary

Maximum range is 1200 mm. `distance` is a valid millimeter reading only when `range_status` is 0.

- Gravity I2C: set the mode DIP switch to I2C and the address DIP switch to 0x52 or 0x53.
- Gravity UART: set the mode DIP switch to UART. 9600 8N1, Modbus unit 50. Depends on DFRobot_RTU.
- Fermion: I2C only, factory address 0x52. `setI2CAddress` changes the address and keeps it after power-off. The same call does nothing on Gravity.

The module is already ranging after power-up. When the host does not drive XSHUT, the module stays enabled.

## Installation

Download this library into the Arduino `libraries` directory, then open a sketch in `examples`.

The Raspberry Pi library is in `python/raspberrypi/`. See the README in that directory.

## Methods

```cpp
  /**
   * @brief Check MODEL_ID
   * @n The module is already ranging after power-up. This reads 0x010F and expects 0xEBAA
   * @n I2C: addr is the 7-bit address. On Gravity, set the address DIP switch to match
   * @n UART: addr is ignored. The Modbus unit is 50
   * @return 1 on success, 0 on failure
   */
  int InitSensor(uint8_t addr = 0x52);

  /**
   * @brief Confirm the sensor can be read
   * @n Checks that MODEL_ID is 0xEBAA. The module is already ranging after power-up
   * @return 1 when the id matches, 0 on failure
   */
  int sensorInit(void);

  /**
   * @brief Read this library's version, not the module's internal version
   * @return 1 on success, 0 if the pointer is null
   */
  int getVersion(VL53L4CD_Version_t *pVersion);

  /**
   * @brief Read the device id
   * @n Register 0x010F. VL53L4CD returns 0xEBAA
   * @return 1 on success, 0 on failure
   */
  int getSensorId(uint16_t *p_id);

  /**
   * @brief Start continuous ranging
   * @n Writes 0x21 to 0x0087. Call this only when ranging must be started again
   * @return 1 on success, 0 on a bus failure
   */
  int startRanging(void);

  /**
   * @brief Stop ranging
   * @n Writes 0x80 to 0x0087
   * @return 1 on success, 0 on a bus failure
   */
  int stopRanging(void);

  /**
   * @brief Read the current measurement
   * @n A return of 1 means the result was read. distance is valid only when range_status is 0
   * @return 1 on success, 0 on a bus failure
   */
  int getResult(VL53L4CD_Result_t *pResult);

  /**
   * @brief Set the ranging offset in millimeters, from -1024 to 1023
   * @return 1 on success, 0 on failure
   */
  int setOffset(int16_t offset_mm);

  /**
   * @brief Read the current offset in millimeters
   * @return 1 on success, 0 on failure
   */
  int getOffset(int16_t *p_offset_mm);

  /**
   * @brief Calibrate the offset against a known distance
   * @n Keep the target still. The offset is cleared, then nb_samples valid ranges are averaged
   * @return 1 when the new offset is stored, 0 on failure
   */
  int calibrateOffset(int16_t target_mm, int16_t *p_measured_offset_mm, int16_t nb_samples);

  /**
   * @brief Set the signal threshold in kcps. Maximum is 16384
   * @n range_status is 2 when the return signal is below this value
   * @return 1 on success, 0 on failure
   */
  int setSignalThreshold(uint16_t signal_kcps);

  /**
   * @brief Read the signal threshold in kcps
   * @return 1 on success, 0 on failure
   */
  int getSignalThreshold(uint16_t *p_signal_kcps);

  /**
   * @brief Set the sigma threshold in millimeters. Maximum is 16383
   * @n range_status is 1 when the measured sigma is above this value
   * @return 1 on success, 0 on failure
   */
  int setSigmaThreshold(uint16_t sigma_mm);

  /**
   * @brief Read the sigma threshold in millimeters
   * @return 1 on success, 0 on failure
   */
  int getSigmaThreshold(uint16_t *p_sigma_mm);

  /**
   * @brief Set the GPIO1 distance window
   * @n window 0: below low_mm
   * @n window 1: above high_mm
   * @n window 2: below low_mm or above high_mm
   * @n window 3: between low_mm and high_mm
   * @return 1 on success, 0 on failure
   */
  int setDetectionThresholds(uint16_t low_mm, uint16_t high_mm, uint8_t window);

  /**
   * @brief Read the GPIO1 distance window
   * @return 1 on success, 0 on failure
   */
  int getDetectionThresholds(uint16_t *p_low_mm, uint16_t *p_high_mm, uint8_t *p_window);

  /**
   * @brief Change the I2C address
   * @n Fermion stores the new address. Gravity keeps the DIP-switch address and the call returns 0
   * @return 1 when the address changed, 0 when rejected or failed
   */
  virtual int setI2CAddress(uint8_t addr);

  /**
   * @brief I2C constructor. xshut_pin -1 does not drive a pin, and the module stays enabled
   */
  DFRobot_VL53L4CD_I2C(TwoWire *i2c = &Wire, int xshut_pin = -1);
  void powerOn(void);
  void powerOff(void);
  int begin(void);

  /**
   * @brief Gravity UART constructor. Do not use this class with Fermion
   * @n The module stays at 9600 8N1, unit 50. Open the serial port at 9600 before begin()
   */
  explicit DFRobot_VL53L4CD_UART(Stream *serial, uint32_t baudRate = 9600);
  int begin(void);
```

## Compatibility

| MCU | Work Well | Work Wrong | Untested | Remarks |
| --- | :--: | :----: | :----: | --- |
| Arduino UNO | √ |  |  |  |
| Arduino Mega2560 | √ |  |  |  |
| Arduino Leonardo |  |  | √ |  |
| Raspberry Pi |  |  | √ | I2C only, see python/raspberrypi |

## History

- 2026/10/09 - V1.0.0

## Credits

Written by wxzed(xiao.wu@dfrobot.com), 2026. (Welcome to our [website](https://www.dfrobot.com/))
