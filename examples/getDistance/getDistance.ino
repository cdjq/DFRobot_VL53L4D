/*!
 * @file getDistance.ino
 * @brief Gravity VL53L4CD I2C 模式连续测距（UNO：SDA=A4, SCL=A5）
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd
 * @n 模块拨码：I2C 模式（PC5 高）；地址拨码 0x52（PC6 高）或 0x53（PC6 低）
 */
#include <Wire.h>
#include <DFRobot_VL53L4CD.h>

/** 与模块拨码一致：默认 0x52；拨到 0x53 时改为 0x53 */
static const uint8_t kModuleI2cAddr = 0x52;

DFRobot_VL53L4CD_I2C tof(&Wire, -1);

void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  Serial.println(F("VL53L4CD I2C getDistance"));

  if (!tof.begin()) {
    Serial.println(F("begin failed"));
    while (true) {
      delay(1000);
    }
  }

  if (!tof.InitSensor(kModuleI2cAddr)) {
    Serial.println(F("InitSensor failed (check addr / wiring / I2C mode)"));
    while (true) {
      delay(1000);
    }
  }

  uint16_t id = 0;
  tof.getSensorId(&id);
  Serial.print(F("Sensor ID: 0x"));
  Serial.println(id, HEX);

  /* CS32 固件上电已 StartRanging，此处可不调用；无固件时可 tof.startRanging() */
  Serial.println(F("Ranging..."));
}

void loop()
{
  VL53L4CD_Result_t r;

  if (tof.getResult(&r)) {
    if (r.range_status == 0) {
      Serial.print(F("mm="));
      Serial.print(r.distance);
    } else {
      Serial.print(F("invalid mm="));
      Serial.print(r.distance);
    }
    Serial.print(F(" status="));
    Serial.print(r.range_status);
    Serial.print(F(" signal="));
    Serial.println(r.signal_rate_kcps);
  } else {
    Serial.println(F("I2C read fail"));
  }
  delay(200);
}
