#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <U8g2lib.h>
#include <inttypes.h>
#include "./type.hpp"

#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//타이머 계산용 상수
const uint64_t TIME_TO_SLEEP = 10;
const uint64_t uS_TO_S_FACTOR = 1000;
const uint64_t mS_TO_S_FACTOR = 1000 * 1000;

//핀 정보 상수들
const int SENSOR_RX_PIN = GPIO_NUM_35;
const int POWER_HOLD_PIN = GPIO_NUM_32;
const uint64_t FUNCTION_BUTTON_PIN = BIT64(GPIO_NUM_34);
const gpio_num_t POWER_BUTTON_PIN = GPIO_NUM_33;
const int BUZZER_PIN = GPIO_NUM_13;
const int MOTOR_PIN = GPIO_NUM_15;
const int NOT_USED_PIN = GPIO_NUM_39;

#endif