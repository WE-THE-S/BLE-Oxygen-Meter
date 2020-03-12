#ifndef __SENSOR_TASK_HPP__
#define __SENSOR_TASK_HPP__
#include <Arduino.h>
#include <esp_err.h>
#include "../config.hpp"
#include "../lcd.hpp"
#include "../type.hpp"

static esp_err_t readSensor(sensor_t *sensor) {
    while(!Serial2.available());
	String recv = Serial2.readStringUntil('\n');
	if (recv.length() > 5) {
		recv.toLowerCase();
		recv = recv.substring(recv.indexOf('o'));
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

static void __sensor_task(void *argv) {
	LCD* lcd = reinterpret_cast<LCD *>(argv);
	ESP_LOGI("Sensor", "Sensor Recv");

	BLE ble;
	if(status.wakeupCount == BROADCAST_INTERVAL_TIME){
		ble.broadcast();
		ble.update(&(status.sensor));
		status.wakeupCount = 0;
	}else{
		status.wakeupCount++;
	}

	while(true){
		sensor_t temp;
		temp.isOk = (readSensor(&temp) == ESP_OK);
		status.sensor.barometric = temp.barometric;
		status.sensor.ppO2 = temp.ppO2;
		status.sensor.o2 = temp.o2;
		status.sensor.temp = temp.temp;
		status.sensor.isOk = temp.isOk;
		//ble.update(&(status.sensor));
		lcd->print();
	}
}

#endif