/*!
 * @file getDistanceUart.ino
 * @brief Gravity UART 连续测距。USB 串口打印，模块走 SoftwareSerial
 * @n 拨码拨到 UART。模块 9600 8N1，Modbus 从站 50
 * @n 下方引脚与 SoftwareSerial(RX=10, TX=11) 一致。Mega 若改接 Serial3，用 getDistanceUartMega
 * @n -----------------------------------------------------------------
 * @n  Board       VCC     GND     MCU TX -> 模块 RX     MCU RX <- 模块 TX
 * @n  UNO         VCC     GND     11                    10
 * @n  Mega2560    VCC     GND     11                    10
 * @n  Leonardo    VCC     GND     11                    10
 * @n -----------------------------------------------------------------
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-09
 * @url https://github.com/DFRobot/DFRobot_VL53L4CD
 */
#include <SoftwareSerial.h>
#include <DFRobot_VL53L4CD.h>

SoftwareSerial sensorSerial(10, 11);
DFRobot_VL53L4CD_UART tof(&sensorSerial, 9600);

void setup()
{
  uint16_t id = 0;

  /* USB 串口 115200 看结果。模块 UART 单独用 9600，两路不要混用。 */
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  sensorSerial.begin(9600);
  /* InitSensor 的地址参数在 UART 下忽略，从站固定 50。 */
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

  /* status 为 0 表示本次距离有效。读失败时打印 uart read fail。 */
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
