#ifndef __SENSOR_TASK_HPP__
#define __SENSOR_TASK_HPP__
#include <Arduino.h>
#include "../config.hpp"
#include "../lcd.hpp"
#include "../type.hpp"
#include "../communication.hpp"

static void __sensor_task(void *argv) {
	LCD *lcd = reinterpret_cast<LCD *>(argv);
	lcd->status->sensorTaskStatus = RUNNING;
	Communication comm;
	comm.begin();
	sensor_t temp;
	comm.readSensor(&temp);
	ESP_LOGI("Sensor Recv");
	if(temp.isOk){
		memcpy(&lcd->status->sensor, &temp, sizeof(sensor_t));
	}else{
		lcd->status->sensor.isOk = false;
	}
	lcd->status->sensorTaskStatus = FINISH;
    vTaskDelete(NULL);
}

#endif