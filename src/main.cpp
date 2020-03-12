#include "./config.hpp"
#include "./lcd.hpp"
#include "./task/button.hpp"
#include "./task/sensor.hpp"
#include "./type.hpp"
#include "util.hpp"

#include <Arduino.h>
#include <HardwareSerial.cpp>
#include <U8g2lib.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//LCD 인스턴스
U8G2_SSD1327_WS_128X128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/OLED_CS_PIN, /* dc=*/OLED_DC_PIN, /* reset=*/OLED_RESET_PIN);

LCD lcd(&u8g2);

void setup() {
	disableCore0WDT();
	disableCore1WDT();
	//켜지면 바로 pin 설정 부터 진행
	pinMode(BATTERY_ADC_PIN, INPUT);
	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(MOTOR_PIN, OUTPUT);
	pinMode(POWER_BUTTON_PIN, INPUT);
	pinMode(FUNCTION_BUTTON_PIN, INPUT);
	attachInterrupt(digitalPinToInterrupt(FUNCTION_BUTTON_PIN), __function_handler, RISING);
	attachInterrupt(digitalPinToInterrupt(POWER_BUTTON_PIN), __power_handler, RISING);
	whyWakeup();

	Serial.begin(115200);
	Serial2.begin(9600, SERIAL_8N1, SENSOR_RX_PIN, NOT_USED_PIN);
	Serial2.setTimeout(SENSOR_TIMEOUT * US_TO_S_FACTOR);
	ESP_LOGI("Main", "Wakeup Count %u", status.wakeupCount);

	lcd.begin();
	ESP_LOGI("Button Task", "Execute");
	//tskIDLE_PRIORITY
	xTaskCreate(__sensor_task, "sensor", 4096, &lcd
		, tskIDLE_PRIORITY, &(status.sensorTaskHandle));

	configASSERT(status.sensorTaskHandle);

	status.sensor.requestSos = status.sosEnable;
	if (status.sensor.o2 < 19.5) {
		status.sensor.warringO2 = 1;
	} else {
		status.sensor.warringO2 = 0;
	}

	if (!(status.sensor.warringO2 || status.sensor.requestSos)) {
		sleep();
	}
}

void loop() {
	bool alreadyRun = false;
	if (status.sosEnable) {
		if (!alreadyRun) {
			ESP_ERROR_CHECK(rtc_gpio_hold_dis(MOTOR_PIN));
			ESP_ERROR_CHECK(rtc_gpio_init(MOTOR_PIN));
			ESP_ERROR_CHECK(rtc_gpio_set_direction(MOTOR_PIN, RTC_GPIO_MODE_OUTPUT_ONLY));
			ESP_ERROR_CHECK(rtc_gpio_set_level(MOTOR_PIN, status.wakeupCount & 1 ? HIGH : LOW));
			ESP_ERROR_CHECK(gpio_hold_en(MOTOR_PIN));

			ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
			ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
			for (int i = 0; i < 6; i++) {
				ledcWrite(BUZZER_CHANNEL, (i & 1) ? BUZZER_ON : BUZZER_OFF);
				delay(100);
			}
			ledcWrite(BUZZER_CHANNEL, BUZZER_OFF);
			alreadyRun = true;
		}
	} else {
		ESP_ERROR_CHECK(rtc_gpio_hold_dis(MOTOR_PIN));
		ESP_ERROR_CHECK(rtc_gpio_init(MOTOR_PIN));
		ESP_ERROR_CHECK(rtc_gpio_set_direction(MOTOR_PIN, RTC_GPIO_MODE_OUTPUT_ONLY));
		ESP_ERROR_CHECK(rtc_gpio_set_level(MOTOR_PIN, LOW));
		ESP_ERROR_CHECK(gpio_hold_en(MOTOR_PIN));
	}
	lcd.print();
}