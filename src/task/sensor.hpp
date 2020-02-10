#ifndef __SENSOR_TASK_HPP__
#define __SENSOR_TASK_HPP__
#include "../config.hpp"
#include "../type.hpp"
#include "./ITask.hpp"

class SensorTask : public ITask {
public:
	void execute(void *argv) {
	    xTaskCreatePinnedToCore(task, "sensor", 4096, NULL, 1, NULL, SENSOR_TASK_CORE_ID);
	}
	static void task(void *argv) {
		device_status_t *status = reinterpret_cast<device_status_t *>(argv);
		Communication comm;
		comm.begin();
		comm.readSensor(&status->sensor);
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
	}
};

#endif