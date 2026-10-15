/*!
 * @file setThreshold.ino
 * @brief 设置 GPIO1 距离窗口并读回，再打印测距
 * @n 拨码拨到 I2C，地址拨码拨到 0x52。window=3 表示距离落在 100 mm 与 300 mm 之间时触发
 * @n 模块 GPIO1 是焊盘输出，本示例不接这根脚，只写寄存器并读回
 * @n 本示例使用 Wire 默认引脚
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

DFRobot_VL53L4CD_I2C tof(&Wire, -1);

void setup()
{
  uint16_t low = 0;
  uint16_t high = 0;
  uint8_t window = 0;

  /* USB 串口 115200。模块走 I2C，地址 0x52。 */
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
  /* 低阈值 100 mm，高阈值 300 mm，窗口 3：处于两者之间。 */
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

  /* 阈值写在 GPIO1 逻辑里。这里仍打印距离和 range_status，0 为有效测距。 */
  if (tof.getResult(&result)) {
    Serial.print(F("mm="));
    Serial.print(result.distance);
    Serial.print(F(" status="));
    Serial.println(result.range_status);
  }
  delay(200);
}
