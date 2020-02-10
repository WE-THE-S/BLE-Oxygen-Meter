#include "./config.hpp"
#include "./type.hpp"
#include <Arduino.h>
#include <HardwareSerial.cpp>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

extern RTC_DATA_ATTR U8G2_SSD1327_WS_128X128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/14, /* dc=*/5, /* reset=*/19);
extern RTC_DATA_ATTR sensor_t sensor;
extern RTC_DATA_ATTR int wake_count;

void print_wakeup_reason() {
	const char *TAG = "WakeUpTag";
	esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

	switch (wakeup_reason) {
		case ESP_SLEEP_WAKEUP_UNDEFINED: {
			ESP_LOGD(TAG, "BOOT");
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT0: {
			ESP_LOGD(TAG, "Wakeup caused by external signal using RTC_IO");
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT1: {
			ESP_LOGD(TAG, "Wakeup caused by external signal using RTC_CNTL");
			break;
		}
		case ESP_SLEEP_WAKEUP_TIMER: {
			ESP_LOGD(TAG, "Wakeup caused by timer");
			break;
		}
		case ESP_SLEEP_WAKEUP_ULP: {
			ESP_LOGD(TAG, "Wakeup caused by ULP program");
			break;
		}
		default:
			ESP_LOGD(TAG, "Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
			break;
	}
}

void setup() {
	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(MOTOR_PIN, OUTPUT);

	ESP_LOGD(TAG, "enter deep sleep");
	esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
	esp_deep_sleep_start();
}

void loop() {
}