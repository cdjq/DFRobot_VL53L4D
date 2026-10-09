/*!
 * @file calibrateOffset.ino
 * @brief 把传感器对准已知距离的稳定目标后校准偏移
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd
 * @n Gravity I2C：SDA=A4，SCL=A5。目标距离默认 100 mm，采样 20 次
 */
#include <Wire.h>
#include <DFRobot_VL53L4CD.h>

static const int16_t kTargetMm = 100;

DFRobot_VL53L4CD_I2C tof(&Wire, -1);

void setup()
{
  int16_t offset = 0;

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
  Serial.println(F("hold a stable target, calibrating..."));
  if (!tof.calibrateOffset(kTargetMm, 20)) {
    Serial.println(F("calibrate failed"));
    while (true) {
      delay(1000);
    }
  }
  if (tof.getOffset(&offset)) {
    Serial.print(F("offset mm="));
    Serial.println(offset);
  }
}

void loop()
{
  VL53L4CD_Result_t result;

  if (tof.getResult(&result) && result.range_status == 0) {
    Serial.print(F("mm="));
    Serial.println(result.distance);
  }
  delay(200);
}
