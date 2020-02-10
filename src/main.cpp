#include "./communication.hpp"
#include "./config.hpp"
#include "./lcd.hpp"
#include "./task/button.hpp"
#include "./task/sensor.hpp"
#include "./type.hpp"
#include "util.hpp"

#include <Arduino.h>
#include <HardwareSerial.cpp>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//계속 유지되야 하는 데이터들
//디바이스 정보
RTC_DATA_ATTR device_status_t status;
void setup() {
	whyWakeup();
	pinMode(POWER_BUTTON_PIN, INPUT);
	pinMode(FUNCTION_BUTTON_PIN, INPUT);
	pinMode(BATTERY_ADC_PIN, INPUT);
	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(MOTOR_PIN, OUTPUT);
	status.buttonTaskStatus = INIT;
	status.sensorTaskStatus = INIT;
	ESP_LOGI("Main", "Wakeup");
	//LCD 인스턴스
	U8G2_SSD1327_WS_128X128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/OLED_CS_PIN, /* dc=*/OLED_DC_PIN, /* reset=*/OLED_RESET_PIN);

	LCD lcd(&u8g2, &status);
	ESP_LOGI("Button Task", "Execute");
	xTaskCreatePinnedToCore(__button_task, "button", 8192, &lcd, tskIDLE_PRIORITY, NULL, BUTTON_TASK_CORE_ID);
	xTaskCreatePinnedToCore(__sensor_task, "sensor", 8192, &lcd, tskIDLE_PRIORITY, NULL, SENSOR_TASK_CORE_ID);
	while(status.buttonTaskStatus != FINISH && status.sensorTaskStatus != FINISH){
		//ESP_LOGI("wait task", "Wait task");
		delay(10);
	}
	if (lcd.status->alarmEnable) {
		ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(EMERGENCY_SLEEP_TIME * mS_TO_S_FACTOR));
	} else {
		ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(NORMAL_SLEEP_TIME * mS_TO_S_FACTOR));
	}
	ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(POWER_BUTTON_PIN, 1));
	ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT64(FUNCTION_BUTTON_PIN), ESP_EXT1_WAKEUP_ANY_HIGH));

	ESP_LOGI("Sleep", "Go To sleep...");
	esp_deep_sleep_start();
}

void loop() {
}