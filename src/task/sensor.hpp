#ifndef __SENSOR_TASK_HPP__
#define __SENSOR_TASK_HPP__
#include "../config.hpp"
#include "../lcd.hpp"
#include "../type.hpp"
#include "../communication.hpp"

static void __sensor_task(void *argv) {
	LCD *lcd = reinterpret_cast<LCD *>(argv);
    auto status = lcd->status;
	Communication comm;
    comm.begin();
	comm.readSensor(&status->sensor);
    lcd->print();
	status->sensorTaskStatus = RUNNING;

	switch (esp_sleep_get_wakeup_cause()) {
		case ESP_SLEEP_WAKEUP_UNDEFINED: {
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT0: {
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT1: {
			break;
		}
		case ESP_SLEEP_WAKEUP_TIMER: {
			break;
		}
		case ESP_SLEEP_WAKEUP_ULP: {
			break;
		}
		default: {
			break;
		}
	}
	status->sensorTaskStatus = FINISH;
    while(true);
}

#endif