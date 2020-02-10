#ifndef __BUTTON_TASK_HPP__
#define __BUTTON_TASK_HPP__
#include "../config.hpp"
#include "../type.hpp"
#include <driver/rtc_io.h>
#include <driver/uart.h>
#include <esp_sleep.h>
#include <soc/uart_channel.h>

static void __button_task(void *argv) {
	LCD* lcd = reinterpret_cast<LCD*>(argv);
	lcd->status->buttonTaskStatus = RUNNING;
	ESP_LOGI("Button", "Running");
	switch (esp_sleep_get_wakeup_cause()) {
		case ESP_SLEEP_WAKEUP_UNDEFINED: {
			rtc_gpio_init(POWER_HOLD_PIN);
			rtc_gpio_set_direction(POWER_HOLD_PIN, RTC_GPIO_MODE_OUTPUT_ONLY);
			rtc_gpio_set_level(POWER_HOLD_PIN, HIGH);
			gpio_hold_en(POWER_HOLD_PIN);
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT0: {
			int time = pulseIn(POWER_BUTTON_PIN, HIGH);
			if (time > LONG_PRESS_THRSHOLD) {
				rtc_gpio_hold_dis(POWER_HOLD_PIN);
				rtc_gpio_init(POWER_HOLD_PIN);
				rtc_gpio_set_direction(POWER_HOLD_PIN, RTC_GPIO_MODE_OUTPUT_ONLY);
				rtc_gpio_set_level(POWER_HOLD_PIN, LOW);
			}
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT1: {
			int time = pulseIn(FUNCTION_BUTTON_PIN, HIGH);
			if (time > LONG_PRESS_THRSHOLD) {
				
			}else{
				if(lcd->status->menu != oled_menu_t::LAST){
					lcd->status->menu = static_cast<oled_menu_t>(static_cast<int>(lcd->status->menu) + 1);
				}else{
					lcd->status->menu = oled_menu_t::FIRST;
				}
				lcd->print();
			}
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
	lcd->status->buttonTaskStatus = FINISH;
}

#endif