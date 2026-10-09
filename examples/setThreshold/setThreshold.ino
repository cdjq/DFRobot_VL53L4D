/*!
 * @file setThreshold.ino
 * @brief 设置 GPIO1 距离窗口并读回
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd
 * @n Gravity I2C：SDA=A4，SCL=A5。把拨码拨到 I2C 和要使用的地址
 */
#include <Wire.h>
#include <DFRobot_VL53L4CD.h>

DFRobot_VL53L4CD_I2C tof(&Wire, -1);

void setup()
{
  uint16_t low = 0;
  uint16_t high = 0;
  uint8_t window = 0;

  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  if (!tof.begin() || !tof.InitSensor(0x52)) {
    Serial.println(F("init failed"));
    while (true) {
      delay(1000);
    }
  }
  if (!tof.setDetectionThresholds(100, 300, 3)) {
    Serial.println(F("set threshold failed"));
    while (true) {
      delay(1000);
    }
  }
  if (tof.getDetectionThresholds(&low, &high, &window)) {
    Serial.print(F("window="));
    Serial.print(window);
    Serial.print(F(" low="));
    Serial.print(low);
    Serial.print(F(" high="));
    Serial.println(high);
  }
}

void loop()
{
  VL53L4CD_Result_t result;

  if (tof.getResult(&result)) {
    Serial.print(F("mm="));
    Serial.print(result.distance);
    Serial.print(F(" status="));
    Serial.println(result.range_status);
  }
  delay(200);
}
