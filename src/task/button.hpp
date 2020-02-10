#ifndef __BUTTON_TASK_HPP__
#define __BUTTON_TASK_HPP__
#include "../config.hpp"
#include "../type.hpp"
#include "./ITask.hpp"

class ButtonTask : public ITask {
public:
	void execute(void *argv) {
	    xTaskCreatePinnedToCore(task, "button", 4096, NULL, 1, NULL, BUTTON_TASK_CORE_ID);
	}
	static void task(void *argv) {
		device_status_t *status = reinterpret_cast<device_status_t *>(argv);
		Communication comm;
		comm.begin();
		comm.readSensor(&status->sensor);
		status->buttonTaskStatus = RUNNING;

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
		status->buttonTaskStatus = FINISH;
	}
};

#endif