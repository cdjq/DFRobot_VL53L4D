/*!
 * @file calibrateOffset.ino
 * @brief 对准已知距离的稳定目标，校准并打印偏移
 * @n 目标距离 kTargetMm 默认 100 mm，有效测距采样 20 次。校准过程中不要移动目标
 * @n 拨码拨到 I2C，地址拨码拨到 0x52。本示例使用 Wire 默认引脚
 * @n -----------------------------------------------------------------
 * @n  Board       VCC     GND     SCL     SDA
 * @n  UNO         VCC     GND     A5      A4
 * @n  Mega2560    VCC     GND     21      20
 * @n  Leonardo    VCC     GND     3       2
 * @n -----------------------------------------------------------------
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-09
 * @url https://github.com/DFRobot/DFRobot_VL53L4CD
 */
#include <Wire.h>
#include <DFRobot_VL53L4CD.h>

static const int16_t kTargetMm = 100;

DFRobot_VL53L4CD_I2C tof(&Wire, -1);

void setup()
{
  int16_t offset = 0;

  /* USB 串口 115200。先确认型号，再开始采样。 */
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
  /* 先把偏移清零，再对有效测距取平均。offset 收到写入后的毫米值。 */
  if (!tof.calibrateOffset(kTargetMm, &offset, 20)) {
    Serial.println(F("calibrate failed"));
    while (true) {
      delay(1000);
    }
  }
  Serial.print(F("offset mm="));
  Serial.println(offset);
}

void loop()
{
  VL53L4CD_Result_t result;

  /* 只打印 range_status 为 0 的有效距离，用来核对校准后的读数。 */
  if (tof.getResult(&result) && result.range_status == 0) {
    Serial.print(F("mm="));
    Serial.println(result.distance);
  }
  delay(200);
}
