#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include "./type.hpp"

#define uS_TO_S_FACTOR 1000 * 1000
#define mS_TO_S_FACTOR 1000
#define TIME_TO_SLEEP 10

const int SENSOR_RX_PIN = 32;
const int POWER_HOLD_PIN = 35;
const int FUNCTION_BUTTON_PIN = 34;
const int POWER_BUTTON_PIN = 33;
const int BUZZER_PIN = 13;
const int MOTOR_PIN = 15;

RTC_DATA_ATTR U8G2_SSD1327_WS_128X128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/14, /* dc=*/5, /* reset=*/19);
RTC_DATA_ATTR sensor_t sensor;
RTC_DATA_ATTR int wake_count;

#endif