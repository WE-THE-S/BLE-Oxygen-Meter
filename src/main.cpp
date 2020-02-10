#include "./config.hpp"
#include "./type.hpp"
#include "./lcd.hpp"
#include "./communication.hpp"
#include "./task/button.hpp"
#include "./task/sensor.hpp"

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
	status.buttonTaskStatus = INIT;
	status.sensorTaskStatus = INIT;
	ESP_LOGI("Main", "Wakeup");
	//LCD 인스턴스
	U8G2_SSD1327_WS_128X128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/OLED_CS_PIN, /* dc=*/OLED_DC_PIN, /* reset=*/OLED_RESET_PIN);

	LCD lcd(&u8g2, &status);
	TaskHandle_t buttonTaskHandle, sensorTaskHandle;
	ESP_LOGI("Button Task", "Execute");
	xTaskCreatePinnedToCore(__button_task, "button", 4096, &lcd, 1, &buttonTaskHandle, BUTTON_TASK_CORE_ID);
	if(status.wakeCount != 1){
		status.wakeCount++;
	}
	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(MOTOR_PIN, OUTPUT);
	while(status.buttonTaskStatus != FINISH){
		if(status.sensorTaskStatus != FINISH){
			Communication comm;
			comm.begin();
			comm.readSensor(&status.sensor);
			lcd.begin();
			lcd.print();
			status.sensorTaskStatus = RUNNING;

			switch (esp_sleep_get_wakeup_cause()) {
				case ESP_SLEEP_WAKEUP_UNDEFINED: {
					break;
				}
				case ESP_SLEEP_WAKEUP_EXT0: {
					break;
				}
				case ESP_SLEEP_WAKEUP_EXT1: {
					break;
				}
				case ESP_SLEEP_WAKEUP_TIMER: {
					break;
				}
				case ESP_SLEEP_WAKEUP_ULP: {
					break;
				}
				default: {
					break;
				}
			}
			status.sensorTaskStatus = FINISH;
		}
	}
	vTaskDelete(buttonTaskHandle);
	ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(2 * mS_TO_S_FACTOR));
	ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(POWER_BUTTON_PIN, 1));
	ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT64(FUNCTION_BUTTON_PIN), ESP_EXT1_WAKEUP_ANY_HIGH));

	esp_deep_sleep_start();
}

void loop() {
}