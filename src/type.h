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

#endif