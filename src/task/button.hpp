#ifndef __BUTTON_TASK_HPP__
#define __BUTTON_TASK_HPP__
#include "../config.hpp"
#include "../type.hpp"

static void __button_task(void *argv) {
	LCD *lcd = reinterpret_cast<LCD *>(argv);
    auto status = lcd->status;
	status->buttonTaskStatus = RUNNING;
    ESP_LOGI("Button", "Running");

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
    while(true);
}

#endif