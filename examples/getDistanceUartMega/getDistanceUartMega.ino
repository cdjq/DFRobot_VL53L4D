/*!
 * @file getDistanceUartMega.ino
 * @brief Mega 2560 用 Serial3 读 Gravity UART 测距，USB Serial 打印结果
 * @n 拨码拨到 UART。Serial3 为 9600 8N1，从站 50。监视器打开 USB 串口，115200
 * @n 引脚与 Serial3 一致：TX3=14，RX3=15。UNO 软串口示例是 getDistanceUart
 * @n -----------------------------------------------------------------
 * @n  Board       VCC     GND     MCU TX -> 模块 RX     MCU RX <- 模块 TX
 * @n  Mega2560    VCC     GND     14 (TX3)              15 (RX3)
 * @n -----------------------------------------------------------------
 * @copyright Copyright (c) 2026 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author wxzed(xiao.wu@dfrobot.com)
 * @version V1.0.0
 * @date 2026-10-09
 * @url https://github.com/cdjq/DFRobot_VL53L4D
 */
#include <DFRobot_VL53L4CD.h>

DFRobot_VL53L4CD_UART tof(&Serial3, 9600);

void setup()
{
  uint16_t id = 0;

  /* USB Serial 看结果。Serial3 才接模块，波特率必须是 9600。 */
  Serial.begin(115200);
  Serial3.begin(9600);
  Serial.println(F("Mega2560 Serial3 VL53L4CD"));
  /* UART 不使用 I2C 地址。0x52 只是占位，从站固定 50。 */
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
  Serial.println(F("Ranging..."));
}

void loop()
{
  VL53L4CD_Result_t result;

  /* status 为 0 表示本次距离有效。 */
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
