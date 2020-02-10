#ifndef __TYPE_HPP__
#define __TYPE_HPP__
#include <inttypes.h>
#include <Arduino.h>

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

typedef enum { INIT, RUNNING, FINNISH } task_status_t;
typedef struct {
	task_status_t sensorTaskStatus;
	task_status_t buttonTaskStatus;
	//얼마나 깨어났는지
	int wakeCount;
} device_status_t;

#endif