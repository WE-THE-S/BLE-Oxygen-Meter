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
	comm.readSensor(&lcd->status->sensor);
	lcd->status->sensorTaskStatus = FINISH;
    vTaskDelete(NULL);
}

#endif