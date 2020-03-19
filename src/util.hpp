#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <inttypes.h>
#include <Arduino.h>
#include <esp_task_wdt.h>

#include "./task/button.hpp"
#include "./config.hpp"
#include "esp_bt_device.h"

//LCD 인스턴스
U8G2_SSD1327_WS_128X128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/OLED_CS_PIN, /* dc=*/OLED_DC_PIN, /* reset=*/OLED_RESET_PIN);

LCD lcd(&u8g2);
BLE ble;

static const char* TAG = "Util";
char *barray2hexstr(uint8_t *data, size_t datalen) {
	size_t final_len = datalen * 2;
	char *chrs = (char *)malloc((final_len + 1) * sizeof(*chrs));
	unsigned int j = 0;
	for (j = 0; j < datalen; j++) {
		chrs[2 * j] = (data[j] >> 4) + 48;
		chrs[2 * j + 1] = (data[j] & 15) + 48;
		if (chrs[2 * j] > 57) chrs[2 * j] += 7;
		if (chrs[2 * j + 1] > 57) chrs[2 * j + 1] += 7;
	}
	chrs[2 * j] = '\0';
	return chrs;
}

void waitPowerOn(){
	switch (esp_sleep_get_wakeup_cause()) {
		case ESP_SLEEP_WAKEUP_UNDEFINED: {
			ESP_LOGI(TAG, "Wakeup by undefined source");
			status.powerOn = true;
			//활성화 안돼있던거면 킨 직후임
			rtc_gpio_init(POWER_HOLD_PIN);
			rtc_gpio_set_direction(POWER_HOLD_PIN, RTC_GPIO_MODE_OUTPUT_ONLY);
			rtc_gpio_set_level(POWER_HOLD_PIN, HIGH);
			gpio_hold_en(POWER_HOLD_PIN);
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT0: {
			ESP_LOGI(TAG, "Wakeup by ext0 (Power Button)");
			__power_handler();
			break;
		}
	}
}
void whyWakeup(){
	if(status.wakeupCount == BROADCAST_INTERVAL_TIME){
		status.bleOn = 1;
		status.wakeupCount = 0;
	}else{
		status.bleOn = 0;
	}
	if(status.bleOn){
		ble.broadcast();
		ble.update(&(status.sensor));
	}
	switch (esp_sleep_get_wakeup_cause()) {
		case ESP_SLEEP_WAKEUP_UNDEFINED: {
			ESP_LOGI(TAG, "Wakeup by undefined source");
			//활성화 안돼있던거면 킨 직후임

			rtc_gpio_init(POWER_HOLD_PIN);
			rtc_gpio_set_direction(POWER_HOLD_PIN, RTC_GPIO_MODE_OUTPUT_ONLY);
			rtc_gpio_set_level(POWER_HOLD_PIN, HIGH);
	
			status.powerOn = true;
			gpio_hold_en(POWER_HOLD_PIN);
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT0: {
			ESP_LOGI(TAG, "Wakeup by ext0 (Power Button)");
			__power_handler();
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT1: {
			ESP_LOGI(TAG, "Wakeup by ext1 (Function Button)");
			__function_handler();
			break;
		}
		case ESP_SLEEP_WAKEUP_TIMER: {
			ESP_LOGI(TAG, "Wakeup by timer");
			break;
		}
		case ESP_SLEEP_WAKEUP_ULP: {
			ESP_LOGI(TAG, "Wakeup by ulp program");
			break;
		}
		default: {
			ESP_LOGI(TAG, "Wakeup by unknown source");
			break;
		}
	}
	if(status.sensor.requestSos | status.sensor.warringO2){
		lcd.print();
	}else{
		if((status.wakeupCount % OLED_UPDATE_INTERVAL_TIME) == OLED_UPDATE_TIME){
			lcd.print();
		}
	}
}


inline void sleep(uint64_t ms){
	ESP_LOGI("Sleep", "Go To sleep... %llu ms", ms);

	digitalWrite(RED_LED_PIN, HIGH);
	digitalWrite(GREEN_LED_PIN, HIGH);
	detachInterrupt(digitalPinToInterrupt(FUNCTION_BUTTON_PIN));
	detachInterrupt(digitalPinToInterrupt(POWER_BUTTON_PIN));
	ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(POWER_BUTTON_PIN, LOW));
	ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT64(FUNCTION_BUTTON_PIN), ESP_EXT1_WAKEUP_ANY_HIGH));
	ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(ms * US_TO_MS_FACTOR));
	esp_deep_sleep_start();
}

#endif