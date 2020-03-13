#include "./config.hpp"
#include "./lcd.hpp"
#include "./task/button.hpp"
#include "./task/sensor.hpp"
#include "./type.hpp"
#include "util.hpp"

#include <Arduino.h>
#include <HardwareSerial.cpp>
#include <U8g2lib.h>
#include <esp_task_wdt.h>
#include <pthread.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


static pthread_t sensorThread;
void setup() {
	status.wakeupCount++;
	status.waitSensorData = 1;
	status.waitProcessDone = 1;
	//켜지면 바로 pin 설정 부터 진행
	pinMode(BATTERY_ADC_PIN, INPUT);
	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(MOTOR_PIN, OUTPUT);
	pinMode(POWER_BUTTON_PIN, INPUT);
	pinMode(FUNCTION_BUTTON_PIN, INPUT);
	attachInterrupt(digitalPinToInterrupt(FUNCTION_BUTTON_PIN), __function_handler, RISING);
	attachInterrupt(digitalPinToInterrupt(POWER_BUTTON_PIN), __power_handler, RISING);
	Serial.begin(115200);
	Serial2.begin(9600, SERIAL_8N1, SENSOR_RX_PIN, NOT_USED_PIN);
	Serial2.setTimeout(SENSOR_TIMEOUT * US_TO_S_FACTOR);
	Serial2.setRxBufferSize(256);
	ESP_LOGI("Main", "Wakeup Count %u", status.wakeupCount);
	ESP_LOGI("Button Task", "Execute");
	//tskIDLE_PRIORITY
	whyWakeup();
	if(pthread_create(&sensorThread, NULL, sensorTask, (void*)nullptr)){
		ESP_LOGE("Thread", "sensor thread create error");
	}
	ESP_LOGI("Process", "Process Done");
	pthread_detach(sensorThread);
}

void loop() {
	if(!status.waitSensorData){
		if(pthread_create(&sensorThread, NULL, sensorTask, (void*)nullptr)){
			ESP_LOGE("Thread", "create error");
		}
		pthread_detach(sensorThread);
	}
	ESP_ERROR_CHECK(rtc_gpio_hold_dis(MOTOR_PIN));
	ESP_ERROR_CHECK(rtc_gpio_init(MOTOR_PIN));
	ESP_ERROR_CHECK(rtc_gpio_set_direction(MOTOR_PIN, RTC_GPIO_MODE_OUTPUT_ONLY));
	if (status.sensor.warringO2 | status.sensor.requestSos) {
		const uint64_t pendingTime = millis() / 1000;
		ESP_ERROR_CHECK(rtc_gpio_set_level(MOTOR_PIN, pendingTime & 1 ? HIGH : LOW));
		ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
		ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
		for (int i = 0; i < 6; i++) {
			ledcWrite(BUZZER_CHANNEL, (i & 1) ? BUZZER_ON : BUZZER_OFF);
			delay(100);
		}
		ledcWrite(BUZZER_CHANNEL, BUZZER_OFF);
	} else {
		ESP_ERROR_CHECK(rtc_gpio_set_level(MOTOR_PIN, LOW));
		sleep(NORMAL_SLEEP_TIME_MS);
	}
	ESP_ERROR_CHECK(gpio_hold_en(MOTOR_PIN));
	lcd.print();
}