#ifndef __BUTTON_TASK_HPP__
#define __BUTTON_TASK_HPP__
#include "../config.hpp"
#include "../type.hpp"
#include "../ble.hpp"
#include <driver/rtc_io.h>
#include <driver/uart.h>
#include <esp_sleep.h>
#include <soc/uart_channel.h>

void IRAM_ATTR __function_handler() {
	status.sosEnable = !status.sosEnable;
} 

void IRAM_ATTR __power_handler() {
	status.powerOn = !status.powerOn;
} 

static void __button_task(void *argv) {
	LCD *lcd = reinterpret_cast<LCD *>(argv);
	status.buttonTaskStatus = RUNNING;
	ESP_LOGI("Button", "Button Task Running");
	switch (esp_sleep_get_wakeup_cause()) {
		case ESP_SLEEP_WAKEUP_UNDEFINED: {
			rtc_gpio_init(POWER_HOLD_PIN);
			rtc_gpio_set_direction(POWER_HOLD_PIN, RTC_GPIO_MODE_OUTPUT_ONLY);
			rtc_gpio_set_level(POWER_HOLD_PIN, HIGH);
			status.powerOn = true;
			gpio_hold_en(POWER_HOLD_PIN);
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT0: {
			rtc_gpio_hold_dis(POWER_HOLD_PIN);
			rtc_gpio_init(POWER_HOLD_PIN);
			rtc_gpio_set_direction(POWER_HOLD_PIN, RTC_GPIO_MODE_OUTPUT_ONLY);
			status.powerOn = !status.powerOn;
			rtc_gpio_set_level(POWER_HOLD_PIN, status.powerOn ? HIGH : LOW);
			gpio_hold_en(POWER_HOLD_PIN);
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT1: {
			status.sosEnable = !status.sosEnable;
			ESP_LOGI(TAG, "Alarm %s", status.sosEnable ? "ON" : "OFF");
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
	status.buttonTaskStatus = FINISH;
	vTaskDelete(NULL);
}

#endif