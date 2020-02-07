#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include "./type.h"

#define uS_TO_S_FACTOR 1000 * 1000
#define mS_TO_S_FACTOR 1000
#define TIME_TO_SLEEP 10

const int SENSOR_TX_PIN = 35;
const int SENSOR_RX_PIN = 32;
const int BUZZER_PIN = 13;
const int MOTOR_PIN = 15;

sensor_t sensor;

#endif