#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#pragma once
#include "type.hpp"
#include <U8g2lib.h>
#include <inttypes.h>

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
const uint64_t SENSOR_TIMEOUT = 2;
const uint64_t NORMAL_SLEEP_TIME = 5;
const uint64_t EMERGENCY_SLEEP_TIME = 1;
const uint64_t US_TO_S_FACTOR = 1000;
const uint64_t MS_TO_S_FACTOR = 1000 * 1000;
const uint64_t BROADCAST_INTERVAL_TIME = 60;

//핀 정보 상수들
const gpio_num_t SENSOR_RX_PIN = GPIO_NUM_35;
const gpio_num_t POWER_HOLD_PIN = GPIO_NUM_32;
const gpio_num_t FUNCTION_BUTTON_PIN = GPIO_NUM_34;
const gpio_num_t POWER_BUTTON_PIN = GPIO_NUM_33;
const gpio_num_t BUZZER_PIN = GPIO_NUM_13;
const gpio_num_t MOTOR_PIN = GPIO_NUM_15;
const gpio_num_t NOT_USED_PIN = GPIO_NUM_39;
const gpio_num_t BATTERY_ADC_PIN = GPIO_NUM_25;
const gpio_num_t RED_LED_PIN = GPIO_NUM_21;
const gpio_num_t GREEN_LED_PIN = GPIO_NUM_27;
const gpio_num_t BLUE_LED_PIN = GPIO_NUM_22;
const gpio_num_t LED_ENABLE_PIN = GPIO_NUM_12;

const gpio_num_t OLED_DIN_PIN = GPIO_NUM_23;
const gpio_num_t OLED_CLK_PIN = GPIO_NUM_18;
const gpio_num_t OLED_CS_PIN = GPIO_NUM_14;
const gpio_num_t OLED_DC_PIN = GPIO_NUM_5;
const gpio_num_t OLED_RESET_PIN = GPIO_NUM_19;

const BaseType_t SENSOR_TASK_CORE_ID = 0;

const uint32_t BUZZER_FREQ = 2048;
const uint32_t BUZZER_CHANNEL = 0;
const uint32_t BUZZER_RESOLUTION = 8;
const uint32_t BUZZER_ON = 128;
const uint32_t BUZZER_OFF = 0;




//계속 유지되야 하는 데이터들
//디바이스 정보
extern RTC_DATA_ATTR device_status_t status;

#endif