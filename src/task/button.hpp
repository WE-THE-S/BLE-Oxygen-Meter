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
			gpio_hold_en(POWER_HOLD_PIN);
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT0: {
			if(digitalRead(FUNCTION_BUTTON_PIN) != LOW){
				lcd->status->alarmEnable = !lcd->status->alarmEnable;
			}
			rtc_gpio_hold_dis(POWER_HOLD_PIN);
			rtc_gpio_init(POWER_HOLD_PIN);
			rtc_gpio_set_direction(POWER_HOLD_PIN, RTC_GPIO_MODE_OUTPUT_ONLY);
			rtc_gpio_set_level(POWER_HOLD_PIN, rtc_gpio_get_level(POWER_HOLD_PIN) != HIGH ? HIGH : LOW);
			gpio_hold_en(POWER_HOLD_PIN);
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT1: {
			if(digitalRead(POWER_BUTTON_PIN) != LOW){
				lcd->status->alarmEnable = !lcd->status->alarmEnable;
			}
			if (lcd->status->menu != oled_menu_t::LAST) {
				lcd->status->menu = static_cast<oled_menu_t>(static_cast<int>(lcd->status->menu) + 1);
			} else {
				lcd->status->menu = oled_menu_t::FIRST;
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
	ESP_LOGI("Button", "Button Task Done");
	lcd->print();
	lcd->status->buttonTaskStatus = FINISH;
	vTaskDelete(NULL);
}

#endif