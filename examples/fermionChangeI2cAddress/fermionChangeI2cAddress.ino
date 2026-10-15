/*!
 * @file fermionChangeI2cAddress.ino
 * @brief Fermion 把 7 位 I2C 地址改成 0x53 并读回型号
 * @n 库必须带编译宏 DFERMION，setI2CAddress 才会把地址原样写入 0x0001
 * @n Gravity 用拨码选择 0x52 或 0x53。在 Gravity 上运行会拒绝写入，不要靠本示例改地址
 * @n 本示例使用 Wire 默认引脚，起始地址 0x52
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
  /* USB 串口 115200。先按当前地址 0x52 确认模块在线。 */
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
  /* 写入 7 位地址 0x53。成功后后续读写改走新地址。 */
  if (tof.setI2CAddress(0x53)) {
    Serial.println(F("Fermion address saved as 0x53"));
    uint16_t id = 0;
    if (tof.getSensorId(&id)) {
      Serial.print(F("Sensor ID: 0x"));
      Serial.println(id, HEX);
    }
  } else {
    Serial.println(F("rejected: Gravity uses the address DIP, not setI2CAddress"));
  }
}

void loop()
{
  /* 改地址只做一次，留在 setup。 */
}
