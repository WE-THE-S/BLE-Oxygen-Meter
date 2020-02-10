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
//LCD 인스턴스
U8G2_SSD1327_WS_128X128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/OLED_CS_PIN, /* dc=*/OLED_DC_PIN, /* reset=*/OLED_RESET_PIN);

LCD lcd(&u8g2, &status);
void setup() {
	ITask* buttonTask = new ButtonTask(&status.buttonTaskStatus);
	ITask* sensorTask = new SensorTask(&status.buttonTaskStatus);
	if(status.wakeCount != 1){
		status.wakeCount++;
	}
	lcd.begin();
	lcd.print();
	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(MOTOR_PIN, OUTPUT);
	xTaskCreatePinnedToCore(buttonTask->execute, "button", 4096, NULL, 1, NULL, 0);
	xTaskCreatePinnedToCore(sensorTask->execute, "sensor", 4096, &status.sensor, 1, NULL, 1);
	while(status.buttonTaskStatus != FINISH && status.sensorTaskStatus != FINISH){

	}
	ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(2 * mS_TO_S_FACTOR));
	ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1));
	ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT64(GPIO_NUM_34), ESP_EXT1_WAKEUP_ANY_HIGH));

	esp_deep_sleep_start();
}

void loop() {
}