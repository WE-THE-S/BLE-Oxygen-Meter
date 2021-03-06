#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

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
const uint64_t SENSOR_TIMEOUT = 1;
const uint64_t NORMAL_SLEEP_TIME_MS = 1000;
const uint64_t MS_TO_S_FACTOR = 1000;
const uint64_t US_TO_MS_FACTOR = MS_TO_S_FACTOR;
const uint64_t US_TO_S_FACTOR = 1000 * 1000;
const uint64_t BROADCAST_INTERVAL_TIME = 3600;
const uint64_t OLED_UPDATE_INTERVAL_TIME = 10;
const uint64_t OLED_UPDATE_TIME = 2;
const uint64_t POWER_FLAG_THRESHOLD = 1500;
const uint64_t UPGRADE_FLAG_THRESHOLD = 5 * MS_TO_S_FACTOR;
const uint64_t RESET_SLEEP_TIME = US_TO_MS_FACTOR;
const uint64_t POWER_OFF_SLEEP_TIME = US_TO_S_FACTOR * 3600ULL * 12ULL;

//핀 정보 상수들
const gpio_num_t SENSOR_RX_PIN = GPIO_NUM_35;
const gpio_num_t POWER_HOLD_PIN = GPIO_NUM_32;
const gpio_num_t FUNCTION_BUTTON_PIN = GPIO_NUM_33;
const gpio_num_t POWER_BUTTON_PIN = GPIO_NUM_34;
const gpio_num_t BUZZER_PIN = GPIO_NUM_13;
const gpio_num_t MOTOR_PIN = GPIO_NUM_15;
//15번 모터 핀도 위험하긴 함
//https://github.com/espressif/esptool/wiki/ESP32-Boot-Mode-Selection#other-pins
const gpio_num_t NOT_USED_PIN = GPIO_NUM_39;
const gpio_num_t BATTERY_ADC_PIN = GPIO_NUM_25;
//const gpio_num_t RED_LED_PIN = GPIO_NUM_12;
const gpio_num_t GREEN_LED_PIN = GPIO_NUM_26;
const gpio_num_t RED_LED_PIN = GREEN_LED_PIN;
//기존 RED LED 핀이 MTDI와 겹치면서 부팅 불가 상황에 들어가서 핀 변경

const gpio_num_t OLED_DIN_PIN = GPIO_NUM_23;
const gpio_num_t OLED_CLK_PIN = GPIO_NUM_18;
const gpio_num_t OLED_CS_PIN = GPIO_NUM_14;
const gpio_num_t OLED_DC_PIN = GPIO_NUM_5;
const gpio_num_t OLED_RESET_PIN = GPIO_NUM_19;

//센서 상수
const float O2_SENSOR_WRRRING_HIGH_THRESHOLD = 25.0f;
const float O2_SENSOR_WARRING_1ST_THRESHOLD = 19.5f;
const float O2_SENSOR_WARRING_2ND_THRESHOLD = 19.0f;
const float O2_SENSOR_WARRING_3RD_THRESHOLD = 18.5f;
const BaseType_t SENSOR_TASK_CORE_ID = 0;

//배터리 상수
const float BATTERY_LEVEL_LOW_THRESHOLD = 3.35f;
const float BATTERY_LEVEL_HIGH_THRESHOLD = 4.0f;
const float BATTERY_LEVEL_POWER_OFF_THRESHOLD = 3.32f;
const uint8_t DISPLAY_BATTERY_LOW_THRESHOLD = 5;
const uint8_t NEED_DISPLAY_BATERRY_LEVEL_THRESHOLD = 10;
const uint64_t LCD_ON_TIME = 5;
const uint64_t LCD_ON_TIMESTAMP = LCD_ON_TIME * MS_TO_S_FACTOR;
const uint8_t LCD_ON_COUNT = LCD_ON_TIMESTAMP / NORMAL_SLEEP_TIME_MS;

//부저 관련 상수
const uint32_t BUZZER_FREQ = 2800;
const uint32_t BUZZER_CHANNEL = 0;
const uint32_t BUZZER_RESOLUTION = 8;
const uint32_t BUZZER_ON = 128;
const uint32_t BUZZER_OFF = 0;

//경고 관련 상수
const uint8_t WARRING_FLAG_THRESHOLD = 3;
const uint8_t WARRING_FLAG_RESET_THREHOLD = 15;

//펌웨어 정보
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define FIRMWARE_ID __DATE__ "[" __TIME__ "] " STR(FIRMWARE_VERSION)

//디바이스 정보
extern RTC_DATA_ATTR device_status_t status;

#endif