#ifndef __BUTTON_TASK_HPP__
#define __BUTTON_TASK_HPP__
#include "../config.hpp"
#include "../type.hpp"
#include <driver/rtc_io.h>
#include <driver/uart.h>
#include <esp_sleep.h>
#include <soc/uart_channel.h>

static void __button_task(void *argv) {
	LCD *lcd = reinterpret_cast<LCD *>(argv);
	lcd->begin();
	lcd->status->buttonTaskStatus = RUNNING;
	ESP_LOGI("Button", "Button Task Running");
	switch (esp_sleep_get_wakeup_cause()) {
		case ESP_SLEEP_WAKEUP_UNDEFINED: {
			rtc_gpio_init(POWER_HOLD_PIN);
			rtc_gpio_set_direction(POWER_HOLD_PIN, RTC_GPIO_MODE_OUTPUT_ONLY);
			rtc_gpio_set_level(POWER_HOLD_PIN, HIGH);
			lcd->status->powerOn = true;
			gpio_hold_en(POWER_HOLD_PIN);
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT0: {
			rtc_gpio_hold_dis(POWER_HOLD_PIN);
			rtc_gpio_init(POWER_HOLD_PIN);
			rtc_gpio_set_direction(POWER_HOLD_PIN, RTC_GPIO_MODE_OUTPUT_ONLY);
			lcd->status->powerOn = !lcd->status->powerOn;
			if(lcd->status->powerOn){
				rtc_gpio_set_level(POWER_HOLD_PIN, HIGH);
			}else{
				rtc_gpio_set_level(POWER_HOLD_PIN, LOW);
			}
			gpio_hold_en(POWER_HOLD_PIN);
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT1: {
			lcd->status->alarmEnable = !lcd->status->alarmEnable;
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
	ESP_LOGI("Button", "Button Task Done");
	lcd->status->buttonTaskStatus = FINISH;
	vTaskDelete(NULL);
}

#endif