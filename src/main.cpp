#include "./config.hpp"
#include "./lcd.hpp"
#include "./task/button.hpp"
#include "./task/sensor.hpp"
#include "./type.hpp"
#include "./util.hpp"

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
	ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
	ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
	ledcWrite(BUZZER_CHANNEL, BUZZER_OFF);
	whyReset();
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
	digitalWrite(MOTOR_PIN, LOW);
	//PULL_DOWN
	attachInterrupt(digitalPinToInterrupt(FUNCTION_BUTTON_PIN), __function_handler, FALLING);
	//PULL_UP
	attachInterrupt(digitalPinToInterrupt(POWER_BUTTON_PIN), __power_handler, RISING);
	Serial2.begin(9600, SERIAL_8N1, SENSOR_RX_PIN, NOT_USED_PIN);
	Serial2.setTimeout(SENSOR_TIMEOUT * US_TO_S_FACTOR);
	Serial2.setRxBufferSize(256);
	if(status.OTAMode){
		auto ota = OTA::getInstance();
		ota->start();
	}
	
	if (status.powerOn) {
		status.waitFirstSensorData = 1;
		status.waitSensorData = 1;
		status.waitProcessDone = 1;
		status.wakeupCount++;
		//3.860742 MAX CHARGE
		//3.82 MAX
		//3.3V 이하면 동작 안함
		ESP_LOGI("Main", "Wakeup Count %u", status.wakeupCount);
		//tskIDLE_PRIORITY
		whyWakeup();
		if (pthread_create(&sensorThread, NULL, sensorTask, (void *)nullptr)) {
			ESP_LOGE("Thread", "sensor thread create error");
			ESP.restart();
		}else{
			if(pthread_detach(sensorThread) != 0){
				ESP_LOGE("Thread", "detach error");
				ESP.restart();
			}
		}
		ESP_LOGI("Process", "Process Done");
	} else {
		waitPowerOn();
		if (status.powerOn) {
			sleep(RESET_SLEEP_TIME);
		} else {
			status.wakeupCount = 0;
			lcd.clear();
			//12시간
			sleep(POWER_OFF_SLEEP_TIME);
		}
	}
}

void loop() {
	digitalWrite(MOTOR_PIN, LOW);
	if (status.waitFirstSensorData != 1) {
		ESP_LOGI("loop", "loop flag : %u", status.sensor.flag);
		battery_check();
		if (status.sensor.flag > 1) {
			if(status.sensor.requestSos){
				status.alarmLevel = UNSAFE;
			}
			alarm(status.alarmLevel, status.sensor);
		} else {
			digitalWrite(MOTOR_PIN, LOW);
			sleep(NORMAL_SLEEP_TIME_MS);
		}

		if (!status.waitSensorData) {
			if (pthread_create(&sensorThread, NULL, sensorTask, (void *)nullptr)) {
				ESP_LOGE("Thread", "create error");
				ESP.restart();
			}else{
				if(pthread_detach(sensorThread) != 0){
					ESP_LOGE("Thread", "detach error");
					ESP.restart();
				}
			}
		}
	} else {
		delay(500);
	}
	if(status.lcdOnWakeupTimestamp + LCD_ON_TIMESTAMP < millis()){
		status.lcdOnWakeupTimestamp = 0;
		status.lcdOnWakeupCount = 0;
		status.needLcdOn = 0;
	}
}