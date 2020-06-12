#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <Arduino.h>
#include <esp_task_wdt.h>
#include <soc/rtc_cntl_reg.h>
#include "esp_bt_device.h"
#include <inttypes.h>

#include "./config.hpp"
#include "./task/button.hpp"

//LCD 인스턴스
U8G2_SSD1327_WS_128X128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/OLED_CS_PIN, /* dc=*/OLED_DC_PIN, /* reset=*/OLED_RESET_PIN);

LCD lcd(&u8g2);
BLE ble;

esp_err_t battery_check();
inline void sleep(uint64_t ms) {
	ESP_LOGI("Sleep", "Go To sleep... %llu ms", ms);
	digitalWrite(RED_LED_PIN, HIGH);
	digitalWrite(GREEN_LED_PIN, HIGH);
	digitalWrite(MOTOR_PIN, LOW);
	digitalWrite(BUZZER_PIN, LOW);
	detachInterrupt(digitalPinToInterrupt(FUNCTION_BUTTON_PIN));
	detachInterrupt(digitalPinToInterrupt(POWER_BUTTON_PIN));
	ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(POWER_BUTTON_PIN, HIGH));
	ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT64(FUNCTION_BUTTON_PIN), ESP_EXT1_WAKEUP_ALL_LOW));
	ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(ms * US_TO_MS_FACTOR));
	esp_deep_sleep_start();
}

void whyReset(){
	ESP_LOGI("Version", "FW Version : %u", FIRMWARE_VERSION);
	const uint8_t *addr = esp_bt_dev_get_address();
	for(uint_fast8_t i = 0;i<3;i++){
		uint_fast16_t value = static_cast<uint_fast16_t>(addr[i + 1]);
		value |= (static_cast<uint_fast16_t>(addr[i]) << 8);
		status.ssid ^= value;
	}
	switch(esp_reset_reason()){
		case ESP_RST_BROWNOUT : {
			status.powerOn = false;
			sleep(RESET_SLEEP_TIME);
			break;
		}
		case ESP_RST_SW : 
			ESP_LOGE("Reset", "SW Reset");
		case ESP_RST_PANIC : {
			status.powerOn = true;
			sleep(RESET_SLEEP_TIME);
			break;
		}		
		default : {
			ESP_LOGE("Reset", "%d", esp_reset_reason());
			break;
		}
	}
}

void waitPowerOn() {
	adc_power_off();
	switch (esp_sleep_get_wakeup_cause()) {
		case ESP_SLEEP_WAKEUP_UNDEFINED: {
			ESP_LOGI(TAG, "Wakeup by undefined source");
			status.powerOn = true;
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT0: {
			ESP_LOGI(TAG, "Wakeup by ext0 (Power Button)");
			__power_handler();
			break;
		}
		default : break;
	}

}
void whyWakeup() {
	if(status.needLcdOn){
		if(status.ledOnWakeupCount + LCD_ON_COUNT > status.wakeupCount){
			status.needLcdOn = 0;
			status.ledOnWakeupCount = 0;
		}
	}
	if (status.wakeupCount == BROADCAST_INTERVAL_TIME) {
		status.bleOn = 1;
		status.wakeupCount = 0;
	} else {
		status.bleOn = 0;
	}
	if (status.bleOn) {
		ble.broadcast();
		ble.update(&(status.sensor));
	}
	switch (esp_sleep_get_wakeup_cause()) {
		case ESP_SLEEP_WAKEUP_UNDEFINED: {
			ESP_LOGI(TAG, "Wakeup by undefined source");
			//활성화 안돼있던거면 킨 직후임
			status.powerOn = true;
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
	battery_check();
	if (status.sensor.requestSos | status.sensor.warringO2High| status.sensor.warringO2Low) {
		lcd.print();
	} else {
		if ((status.wakeupCount % OLED_UPDATE_INTERVAL_TIME) == OLED_UPDATE_TIME) {
			lcd.print();
		}
	}
}

esp_err_t battery_check(){
	pinMode(POWER_HOLD_PIN, OUTPUT);
	digitalWrite(POWER_HOLD_PIN, HIGH);
	adc_power_on();
    int read_raw;
    ESP_ERROR_CHECK(adc2_config_channel_atten(ADC2_CHANNEL_8, ADC_ATTEN_11db));
    ESP_ERROR_CHECK(adc2_get_raw(ADC2_CHANNEL_8, ADC_WIDTH_12Bit, &read_raw));
	adcAttachPin(BATTERY_ADC_PIN);
	double bat = static_cast<double>(read_raw);
	bat = min((bat / 4096 * 3.3), 4096.0) * 2;
	ESP_LOGI("Battery", "%g V", bat);
	if(bat > BATTERY_LEVEL_POWER_OFF_THRESHOLD){
		float level = ((bat - BATTERY_LEVEL_LOW_THRESHOLD) / (BATTERY_LEVEL_HIGH_THRESHOLD - BATTERY_LEVEL_LOW_THRESHOLD)) * 100.0f;
		ESP_LOGI("Battery", "update raw Level %d %%", read_raw);
		ESP_LOGI("Battery", "update raw Level double %f %%", level);
		level = min(level, 100.0f);
		level = max(level, 0.0f);
		status.batteryLevel = static_cast<uint8_t>(level);
		ESP_LOGI("Battery", "Level %u %%", status.batteryLevel);
	}else{
		status.batteryLevel = 0;
		status.sosEnable = 0;
		status.powerOn = 0;
		status.sensor.warringO2Low = 0;
		status.sensor.warringO2High = 0;
		status.sensor.requestSos = 0;
		lcd.clear();
		sleep(POWER_OFF_SLEEP_TIME);
	}
	adc_power_off();
	digitalWrite(POWER_HOLD_PIN, LOW);
	return ESP_OK;
}
esp_err_t alarm(const alarm_status_t alarm, const sensor_t sensor){
	if(alarm != SAFE){
		ESP_ERROR_CHECK(ble.broadcast());
		ESP_ERROR_CHECK(ble.update(&sensor));
		lcd.print();
		const uint64_t pendingTime = millis() / 1000;
		digitalWrite(MOTOR_PIN, pendingTime & 1 ? HIGH : LOW);
		for (int i = 0; i < (alarm * 2); i++) {
			digitalWrite(RED_LED_PIN, (i & 1) ? LOW : HIGH);
			ledcWrite(BUZZER_CHANNEL, (i & 1) ? BUZZER_ON : BUZZER_OFF);
			delay(100);
		}
		digitalWrite(RED_LED_PIN, HIGH);
		ledcWrite(BUZZER_CHANNEL, BUZZER_OFF);
	}
	return ESP_OK;
}


#endif