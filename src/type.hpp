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
	
	//얼마나 깨어났는지, 최대 63
	uint16_t wakeupCount : 6;

	//BLE가 뿌려져야 하는 시간인지 확인용
	uint16_t bleOn : 1; 

	//켜진 상태 체크
	uint16_t powerOn : 1;
	//======

	//SOS enable
	uint16_t sosEnable : 1;

	sensor_t sensor;
} device_status_t;

#endif