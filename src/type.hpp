#ifndef __TYPE_HPP__
#define __TYPE_HPP__
#include <Arduino.h>
#include <U8g2lib.h>
#include <inttypes.h>

typedef union {
	struct {
		float temp;
		float o2;
		int16_t ppO2;
		int16_t barometric;
		bool isOk;
	};
	uint8_t bytes[];
} sensor_t;

typedef enum {
	INIT,
	RUNNING,
	FINISH
} task_status_t;

 typedef struct {
	task_status_t sensorTaskStatus;
	task_status_t buttonTaskStatus;
	//얼마나 깨어났는지
	bool alarmEnable;
	bool powerOn;
	sensor_t sensor;
} device_status_t;

#endif