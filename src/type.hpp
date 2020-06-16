#ifndef __TYPE_HPP__
#define __TYPE_HPP__
#include <Arduino.h>
#include <U8g2lib.h>
#include <inttypes.h>
#include "./config.hpp"

#pragma pack(push, 1)
/**
 * @struct sensor_t
 * @brief 센서 데이터 구조체
 */
typedef union {
	struct {
		/**
		 * @var float temp
		 * 온도 변수
		*/
		float temp;
		float o2;
		
		int16_t ppO2;
		int16_t barometric;
		
		//아래부터 채워짐 
		uint8_t isOk : 1;
		uint8_t requestSos : 1;
		uint8_t warringO2Low : 1;
		uint8_t warringO2High : 1;
		uint8_t dummy : 4;
	};
	uint8_t bytes[];
} sensor_t;

typedef enum {
	SAFE = 0,
	WARRING_1ST = 1,
	WARRING_2ND = 2,
	WARRING_3RD = 3,

	UNSAFE = WARRING_3RD
} alarm_status_t;

/**
 * @struct device_status_t
 * @brief 기기의 현 상태를 저장하는 구조체
 */
typedef struct {
	// SSID에 포함됄 MAC HASH
	uint32_t ssid;
	
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

	//배터리 전압
	double rawBatteryVoltage;

	//배터리 레벨 (100 단계)
	uint8_t batteryLevel;
	
	uint8_t checkSendBT : 1;

	uint8_t needLcdOn : 1;

	//LCD가 켜졋을때 깨어난 횟수
	uint8_t ledOnWakeupCount : 6;
	//Alarm레벨
	alarm_status_t alarmLevel;

	sensor_t sensor;
} device_status_t;

#pragma pack(pop)
#endif