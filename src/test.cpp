#include <Arduino.h>
#include <driver/rtc_io.h>
#include <driver/uart.h>
#include <esp_sleep.h>
#include <soc/uart_channel.h>

#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define uS_TO_S_FACTOR 1000000 //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 5		   //Time ESP32 will go to sleep (in seconds)

typedef union {
	struct {
		float temp;
		float o2;
		int16_t ppO2;
		int16_t barometric;
		bool isOk;
	};
	uint8_t bytes[];
} sensor_t;

RTC_DATA_ATTR U8G2_SSD1327_WS_128X128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/14, /* dc=*/5, /* reset=*/19);
RTC_DATA_ATTR sensor_t sensor;
RTC_DATA_ATTR int wake_count;

void print_wakeup_reason() {
	rtc_gpio_hold_dis(GPIO_NUM_32);
	rtc_gpio_init(GPIO_NUM_32);
	rtc_gpio_set_direction(GPIO_NUM_32, RTC_GPIO_MODE_OUTPUT_ONLY);
	esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

	switch (wakeup_reason) {
		case ESP_SLEEP_WAKEUP_EXT0: {
			rtc_gpio_set_level(GPIO_NUM_32, HIGH);
			wake_count++;
			Serial.print(wake_count);
			Serial.println("Wakeup caused by external signal using RTC_IO");
			delay(100);
			break;
		}
		case ESP_SLEEP_WAKEUP_EXT1: {
			Serial.println("Wakeup caused by external signal using RTC_CNTL");
			delay(100);
			break;
		}
		case ESP_SLEEP_WAKEUP_TIMER: {
			Serial.println("Wakeup caused by timer");
			rtc_gpio_set_level(GPIO_NUM_32, LOW);
			readSensor();
			wake_count = 0;
			break;
		}
		case ESP_SLEEP_WAKEUP_TOUCHPAD:
			Serial.println("Wakeup caused by touchpad");
			break;
		case ESP_SLEEP_WAKEUP_ULP:
			Serial.println("Wakeup caused by ULP program");
			break;
		default: {
			u8g2.begin();
			Serial.println("Wakeup was not caused by deep sleep");
			rtc_gpio_set_level(GPIO_NUM_32, LOW);
			wake_count = 0;
			break;
		}
	}
	char str[32] = {
		0,
	};
  randomSeed(wake_count);
	gpio_hold_en(GPIO_NUM_32);
	u8g2.setFont(u8g2_font_profont17_tf);
	sprintf(str, "o2 : %.2f", random(300));
	u8g2.drawStr(0, 20, str);
	memset(str, 0x00, 16);
	sprintf(str, "temp : %.2f", random(300));
	u8g2.drawStr(0, 40, str);
	memset(str, 0x00, 16);
	sprintf(str, "ppo2 : %d", random(300));
	u8g2.drawStr(0, 60, str);
	memset(str, 0x00, 16);
	sprintf(str, "bar : %d", random(300));
	u8g2.drawStr(0, 80, str);
	memset(str, 0x00, 16);
	sprintf(str, "isOk : %d", random(300));
	u8g2.drawStr(0, 100, str);
	u8g2.sendBuffer();
}


void loop2(void *pvParameters) {
	Serial.println(xPortGetCoreID());
	while (1) {
		int ctrl = digitalRead(34);
		int io = digitalRead(33);
		//Serial.print(io);
		//Serial.print(", ");
		//Serial.println(ctrl);
	}
}

void loop3(void *pvParameters) {
	Serial.println(xPortGetCoreID());
	print_wakeup_reason();
	ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR));
	ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1));
	ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT64(GPIO_NUM_34), ESP_EXT1_WAKEUP_ANY_HIGH));

	esp_deep_sleep_start();
}

void setup() {
	Serial.begin(115200);
	Serial2.begin(9600, SERIAL_8N1, 35, 31);
	Serial2.setTimeout(500);
	xTaskCreatePinnedToCore(loop2, "loop2", 4096, NULL, 1, NULL, 0);
	xTaskCreatePinnedToCore(loop3, "loop3", 4096, NULL, 1, NULL, 1);
}

void loop() {
	// put your main code here, to run repeatedly:
}