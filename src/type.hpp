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
		
		uint8_t isOk : 1;
		uint8_t requestSos : 1;
		uint8_t warringO2 : 1;
		uint8_t dummy : 5;
	};
	uint8_t bytes[];
} sensor_t;

 typedef struct {
	//얼마나 깨어났는지, 최대 63
	uint16_t wakeupCount : 6;

	//BLE가 뿌려져야 하는 시간인지 확인용
	uint16_t bleOn : 1; 

	//켜진 상태 체크
	uint16_t powerOn : 1;

	//SOS enable
	uint16_t sosEnable : 1;

	//센서 데이터 대기중
	uint16_t waitSensorData : 1;
	
	//통신 모듈 데이터 대기중
	uint16_t waitProcessDone : 1;

	//통신 모듈 초기 데이터 대기중
	uint16_t waitFirstSensorData : 1;

	//warring count
	uint16_t warringCount : 4;
	
	//배터리 레벨
	double batteryLevel;

	sensor_t sensor;
} device_status_t;

#endif