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
void setup() {
	//켜지면 바로 pin 설정 부터 진행
	pinMode(BATTERY_ADC_PIN, INPUT);
	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(MOTOR_PIN, OUTPUT);
	pinMode(POWER_BUTTON_PIN, INPUT);
	pinMode(FUNCTION_BUTTON_PIN, INPUT);
	attachInterrupt(digitalPinToInterrupt(FUNCTION_BUTTON_PIN), __function_handler, RISING);
	attachInterrupt(digitalPinToInterrupt(POWER_BUTTON_PIN), __power_handler, RISING);

	whyWakeup();
	
	Serial.begin(115200);
	Serial2.begin(9600, SERIAL_8N1, SENSOR_RX_PIN, NOT_USED_PIN);
	Serial2.setTimeout(SENSOR_TIMEOUT * US_TO_S_FACTOR);
	status.buttonTaskStatus = INIT;
	status.sensorTaskStatus = INIT;
	if(status.wakeupCount == UINT8_MAX){
		status.wakeupCount = 0;
	}else{
		status.wakeupCount++;
	}
	
	ESP_LOGI("Main", "Wakeup Count %u", status.wakeupCount);
	//LCD 인스턴스
	U8G2_SSD1327_WS_128X128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/OLED_CS_PIN, /* dc=*/OLED_DC_PIN, /* reset=*/OLED_RESET_PIN);

	LCD lcd(&u8g2);
	lcd.begin();
	ESP_LOGI("Button Task", "Execute");
	//tskIDLE_PRIORITY
	xTaskCreatePinnedToCore(__sensor_task, "sensor", 4096, &lcd, 1, NULL, SENSOR_TASK_CORE_ID);
	xTaskCreatePinnedToCore(__button_task, "button", 4096, &lcd, 1, NULL, BUTTON_TASK_CORE_ID);
	bool alreadyRun = false;
	while (status.buttonTaskStatus != FINISH && status.sensorTaskStatus != FINISH) {
		if (status.sosEnable) {
			if (!alreadyRun) {
				BLE ble;
				ble.begin();
				ble.broadcast(&(status.sensor));
				rtc_gpio_hold_dis(MOTOR_PIN);
				rtc_gpio_init(MOTOR_PIN);
				rtc_gpio_set_direction(MOTOR_PIN, RTC_GPIO_MODE_OUTPUT_ONLY);
				rtc_gpio_set_level(MOTOR_PIN, status.wakeupCount & 1 ? HIGH : LOW);
				gpio_hold_en(MOTOR_PIN);

				ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
				ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
				for (int i = 0; i < 6; i++) {
					ledcWrite(BUZZER_CHANNEL, (i & 1) ? BUZZER_ON : BUZZER_OFF);
					delay(100);
				}
				ledcWrite(BUZZER_CHANNEL, BUZZER_OFF);
				alreadyRun = true;
			}
		}else{
			rtc_gpio_hold_dis(MOTOR_PIN);
			rtc_gpio_init(MOTOR_PIN);
			rtc_gpio_set_direction(MOTOR_PIN, RTC_GPIO_MODE_OUTPUT_ONLY);
			rtc_gpio_set_level(MOTOR_PIN, LOW);
			gpio_hold_en(MOTOR_PIN);
		}
		lcd.print();
	}
	if (status.sosEnable) {
		ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(EMERGENCY_SLEEP_TIME * MS_TO_S_FACTOR));
	} else {
		ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(NORMAL_SLEEP_TIME * MS_TO_S_FACTOR));
	}
	ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(POWER_BUTTON_PIN, HIGH));
	ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT64(FUNCTION_BUTTON_PIN), ESP_EXT1_WAKEUP_ANY_HIGH));

	ESP_LOGI("Sleep", "Go To sleep...");
	esp_deep_sleep_start();
}

void loop() {
}