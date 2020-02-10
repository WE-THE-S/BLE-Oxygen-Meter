#ifndef __SENSOR_TASK_HPP__
#define __SENSOR_TASK_HPP__
#include "../config.hpp"
#include "../type.hpp"
#include "./ITask.hpp"

class SensorTask : public ITask {
public:
	void execute() {
		*(this->status) = RUNNING;

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
		*(this->status) = FINISH;
	}
};

#endif