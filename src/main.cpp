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
	//켜지면 바로 pin 설정 부터 진행
	pinMode(BATTERY_ADC_PIN, INPUT);
	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(MOTOR_PIN, OUTPUT);
	pinMode(POWER_BUTTON_PIN, INPUT);
	pinMode(FUNCTION_BUTTON_PIN, INPUT);
	pinMode(GREEN_LED_PIN, OUTPUT);
	pinMode(RED_LED_PIN, OUTPUT);
	digitalWrite(RED_LED_PIN, HIGH);
	digitalWrite(GREEN_LED_PIN, HIGH);
	if (status.powerOn) {
		status.waitFirstSensorData = 1;
		status.wakeupCount++;
		status.waitSensorData = 1;
		status.waitProcessDone = 1;
		//PULL_DOWN
		attachInterrupt(digitalPinToInterrupt(FUNCTION_BUTTON_PIN), __function_handler, RISING);
		//PULL_UP
		attachInterrupt(digitalPinToInterrupt(POWER_BUTTON_PIN), __power_handler, FALLING);
		Serial2.begin(9600, SERIAL_8N1, SENSOR_RX_PIN, NOT_USED_PIN);
		Serial2.setTimeout(SENSOR_TIMEOUT * US_TO_S_FACTOR);
		Serial2.setRxBufferSize(256);
		ESP_LOGI("Main", "Wakeup Count %u", status.wakeupCount);
		ESP_LOGI("Button Task", "Execute");
		//tskIDLE_PRIORITY
		whyWakeup();
		if (pthread_create(&sensorThread, NULL, sensorTask, (void *)nullptr)) {
			ESP_LOGE("Thread", "sensor thread create error");
			ESP.restart();
		}
		pthread_detach(sensorThread);
		ESP_LOGI("Process", "Process Done");
	} else {
		waitPowerOn();
		if (status.powerOn) {
			sleep(1ULL);
		} else {
			status.wakeupCount = 0;
			lcd.clear();
			//12시간
			sleep(MS_TO_S_FACTOR * 3600ULL * 12ULL);
		}
	}
}

void loop() {
	if (status.waitFirstSensorData != 1) {
		ESP_ERROR_CHECK(rtc_gpio_hold_dis(MOTOR_PIN));
		ESP_ERROR_CHECK(rtc_gpio_init(MOTOR_PIN));
		ESP_ERROR_CHECK(rtc_gpio_set_direction(MOTOR_PIN, RTC_GPIO_MODE_OUTPUT_ONLY));
		if (status.sensor.warringO2 | status.sensor.requestSos) {
			ble.broadcast();
			ble.update(&(status.sensor));
			lcd.print();
			const uint64_t pendingTime = millis() / 1000;
			ESP_ERROR_CHECK(rtc_gpio_set_level(MOTOR_PIN, pendingTime & 1 ? HIGH : LOW));
			ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
			ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
			for (int i = 0; i < 6; i++) {
				digitalWrite(RED_LED_PIN, (i & 1 ) ? LOW : HIGH);
				ledcWrite(BUZZER_CHANNEL, (i & 1) ? BUZZER_ON : BUZZER_OFF);
				delay(100);
			}
			digitalWrite(RED_LED_PIN, HIGH);
			ledcWrite(BUZZER_CHANNEL, BUZZER_OFF);
		} else {
			ESP_ERROR_CHECK(rtc_gpio_set_level(MOTOR_PIN, LOW));
			sleep(NORMAL_SLEEP_TIME_MS);
		}
		ESP_ERROR_CHECK(gpio_hold_en(MOTOR_PIN));
		if (!status.waitSensorData) {
			if (pthread_create(&sensorThread, NULL, sensorTask, (void *)nullptr)) {
				ESP_LOGE("Thread", "create error");
			}
			pthread_detach(sensorThread);
		}
	} else {
		delay(500);
	}
}