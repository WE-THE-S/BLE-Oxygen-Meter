#ifndef __SENSOR_TASK_HPP__
#define __SENSOR_TASK_HPP__
#include <Arduino.h>
#include <esp_err.h>
#include <esp_task_wdt.h>
#include "../config.hpp"
#include "../lcd.hpp"
#include "../type.hpp"
#include "../util.hpp"

static esp_err_t readSensor(sensor_t *sensor) {
	while (!Serial2.available())
		;
	String recv = Serial2.readStringUntil('\n');
	recv.toLowerCase();
	recv = recv.substring(recv.indexOf('o'));
	if (recv.length() > 5) {
		char *temp = const_cast<char *>(recv.c_str());
		ESP_LOGD(this->TAG, "%s", recv.c_str());
		char *token = strtok(temp, " ");
		while (token != NULL) {
			//ESP_LOGD(TAG, "%s", token);
			char command = token[0];
			token = strtok(NULL, " ");
			switch (command) {
				case 'e': {
					int code = atoi(token);
					sensor->isOk = (code == 0);
					break;
				}
				case 'o': {
					sensor->ppO2 = static_cast<int16_t>(atof(token));
					break;
				}
				case 't': {
					sensor->temp = atof(token);
					break;
				}
				case 'p': {
					float temp = atof(token);
					sensor->barometric = static_cast<int16_t>(temp);
					break;
				}
				case '%': {
					sensor->o2 = atof(token);
					break;
				}
			}
		}
	} else {
		return ESP_FAIL;
	}
	return ESP_OK;
}

void *sensorTask(void *test) {
	status.waitSensorData = 1;
	ESP_LOGD("Serial2 Event", "Serial2 Event Execute");
	sensor_t temp;
	temp.temp = 0.0f;
	temp.o2 = 0.0f;
	temp.barometric = 0;
	temp.ppO2 = 0;
	const auto prepareTime = millis();
	temp.isOk = (readSensor(&temp) == ESP_OK);
	const auto sensorUseTime = millis() - prepareTime;
	const auto sensorRecvTime = millis();
	status.sensor.barometric = temp.barometric;
	status.sensor.ppO2 = temp.ppO2;
	status.sensor.o2 = temp.o2;
	status.sensor.temp = temp.temp;
	status.sensor.isOk = temp.isOk;
	ESP_LOGI("Sensor", "Request Done");

	status.sensor.requestSos = status.sosEnable;
	if (status.sensor.o2 < O2_SENSOR_THRESHOLD) {
		status.sensor.warringO2 = 1;
	} else {
		status.sensor.warringO2 = 0;
	}
	ESP_LOGI("Sensor", "Prepare Read %lums", prepareTime);
	ESP_LOGI("Sensor", "Sensor Recv Data %lums", sensorRecvTime);
	ESP_LOGI("Sensor", "Sensor Use Data %lums", sensorUseTime);
	ESP_LOGI("Sensor", "offset setup %lums", millis());
	ESP_LOGI("Status", "requestSos : %d", status.sensor.requestSos);
	ESP_LOGI("Status", "warringO2 : %d", status.sensor.warringO2);
	if (!(status.sensor.warringO2 || status.sensor.requestSos)) {
		uint64_t sleepTime = NORMAL_SLEEP_TIME_MS;
		sleep(sleepTime);
	}
	status.waitSensorData = 0;
}
#endif