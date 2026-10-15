/*!
 * @file getDistance.ino
 * @brief Gravity I2C 连续测距，USB 串口打印距离
 * @n 拨码拨到 I2C。地址拨码与 kModuleI2cAddr 一致，默认 0x52
 * @n 本示例使用 Wire 默认引脚。构造里的 xshut_pin 为 -1，不操作主机侧 XSHUT
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

/** 与地址拨码一致。拨到 0x53 时把这里改成 0x53 */
static const uint8_t kModuleI2cAddr = 0x52;

DFRobot_VL53L4CD_I2C tof(&Wire, -1);

void setup()
{
  /* USB 串口只用来看结果，115200。模块通信走 I2C。 */
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  Serial.println(F("VL53L4CD I2C getDistance"));

  /* 打开主机 I2C。未接主机侧 XSHUT 时，模块由板载 CS32 保持使能。 */
  if (!tof.begin()) {
    Serial.println(F("begin failed"));
    while (true) {
      delay(1000);
    }
  }

  /* 核对 MODEL_ID 为 0xEBAA。固件上电已经完成初始化和连续测距。 */
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

  /* range_status 为 0 才是有效毫米。其它状态仍打印读数，方便看超量程或无目标。 */
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
