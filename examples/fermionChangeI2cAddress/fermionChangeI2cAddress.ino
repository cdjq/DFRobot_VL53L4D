/*!
 * @file fermionChangeI2cAddress.ino
 * @brief Fermion 修改 I2C 地址。Gravity 拨码产品会拒绝这次写入
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd
 * @n 仅当库以 DFERMION 编译时 setI2CAddress 才会写 0x0001
 * @n Gravity 把地址拨码拨到对应位置，不要运行本示例去改地址
 */
#include <Wire.h>
#include <DFRobot_VL53L4CD.h>

DFRobot_VL53L4CD_I2C tof(&Wire, -1);

void setup()
{
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
}
