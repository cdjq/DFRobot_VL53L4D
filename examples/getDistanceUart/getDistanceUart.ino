/*!
 * @file getDistanceUart.ino
 * @brief Gravity UART 连续测距。把模式拨码拨到 UART
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd
 *
 * | 板型 | MCU TX → 模块 RX | MCU RX ← 模块 TX |
 * | UNO | TX=11 | RX=10 |
 * SoftwareSerial(RX=10, TX=11)，9600 8N1，从站 50
 */
#include <SoftwareSerial.h>
#include <DFRobot_VL53L4CD.h>

SoftwareSerial sensorSerial(10, 11);
DFRobot_VL53L4CD_UART tof(&sensorSerial, 9600);

void setup()
{
  uint16_t id = 0;

  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  sensorSerial.begin(9600);
  if (!tof.begin() || !tof.InitSensor(0x52)) {
    Serial.println(F("uart init failed"));
    while (true) {
      delay(1000);
    }
  }
  if (tof.getSensorId(&id)) {
    Serial.print(F("Sensor ID: 0x"));
    Serial.println(id, HEX);
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
  } else {
    Serial.println(F("uart read fail"));
  }
  delay(200);
}
