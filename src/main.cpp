#include <Arduino.h>
#include <HardwareSerial.cpp>
#include <cstdlib>
#include <cstring>
#include <esp_log.h>

#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define uS_TO_S_FACTOR 1000 * 1000
#define TIME_TO_SLEEP 10

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory

const int SENSOR_TX_PIN = 35;
const int SENSOR_RX_PIN = 32;

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

//9A99D541 B81E9341 01 00B2 00F7
esp_ble_adv_data_t adv_config = {
	.set_scan_rsp = false,
	.include_name = true,
	.include_txpower = true,
	.min_interval = 512,
	.max_interval = 1024,
	.appearance = 0,
	.manufacturer_len = 0,
	.p_manufacturer_data = NULL,
	.service_data_len = 0,
	.p_service_data = NULL,
	.service_uuid_len = 0,
	.p_service_uuid = NULL,
	.flag = (ESP_BLE_ADV_FLAG_NON_LIMIT_DISC)};

esp_ble_adv_params_t adv_param = {
	.adv_int_min = 512,
	.adv_int_max = 1024,
	.adv_type = ADV_TYPE_NONCONN_IND, // Excelent description of this parameter here: https://www.esp32.com/viewtopic.php?t=2267
	.own_addr_type = BLE_ADDR_TYPE_PUBLIC,
	.peer_addr = {
		0x00,
	},
	.peer_addr_type = BLE_ADDR_TYPE_PUBLIC,
	.channel_map = ADV_CHNL_ALL,
	.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

sensor_t sensor;

// O 0209.0 T +19.8 P 1016 % 020.57 e 0000

char *barray2hexstr(uint8_t *data, size_t datalen) {
	size_t final_len = datalen * 2;
	char *chrs = (char *)malloc((final_len + 1) * sizeof(*chrs));
	unsigned int j = 0;
	for (j = 0; j < datalen; j++) {
		chrs[2 * j] = (data[j] >> 4) + 48;
		chrs[2 * j + 1] = (data[j] & 15) + 48;
		if (chrs[2 * j] > 57) chrs[2 * j] += 7;
		if (chrs[2 * j + 1] > 57) chrs[2 * j + 1] += 7;
	}
	chrs[2 * j] = '\0';
	return chrs;
}


void print_wakeup_reason() {
  const char* TAG = "WakeUpTag";
  esp_sleep_wakeup_cause_t wakeup_reason;

	wakeup_reason = esp_sleep_get_wakeup_cause();

	switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_UNDEFINED : {
	    u8g2.begin();
      ESP_LOGD(TAG, "BOOT");
      break;
    }
		case ESP_SLEEP_WAKEUP_TIMER:{
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB14_tr);
      u8g2.drawStr(0,20, "Hello World!");
      u8g2.sendBuffer();
			ESP_LOGD(TAG, "Wakeup caused by timer");
			break;
    }
		case ESP_SLEEP_WAKEUP_ULP:{
			ESP_LOGD(TAG, "Wakeup caused by ULP program");
			break;
    }
		default:
			ESP_LOGD(TAG, "Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
			break;
	}
}

void readSensor() {
  const char* TAG= "Sensor";
	Serial2.setTimeout(2000);
	String recv = Serial2.readStringUntil('\n');
	recv.toLowerCase();
	recv = recv.substring(recv.indexOf('o'));
	char *temp = const_cast<char *>(recv.c_str());
	ESP_LOGD(TAG, "%s", temp);
	char *token = strtok(temp, " ");
	while (token != NULL) {
		//ESP_LOGD(TAG, "%s", token);
		char command = token[0];
		token = strtok(NULL, " ");
		switch (command) {
			case 'e': {
				int code = atoi(token);
				sensor.isOk = (code == 0);
				break;
			}
			case 'o': {
				sensor.ppO2 = static_cast<int16_t>(atof(token));
				break;
			}
			case 't': {
				sensor.temp = atof(token);
				break;
			}
			case 'p': {
				float temp = atof(token);
				sensor.barometric = static_cast<int16_t>(temp);
				char *str = barray2hexstr((uint8_t *)&sensor.barometric, static_cast<size_t>(2));
				delete[] str;
				break;
			}
			case '%': {
				sensor.o2 = atof(token);
				break;
			}
		}
	}
}

static void gap_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
	switch (event) {
		case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT: {
			esp_ble_gap_start_advertising(&adv_param);
			break;
		}
		default:
			break;
	}
}

void setup() {
  const char* TAG = "Bluetooth";
	sensor.isOk = false;
	Serial2.begin(9600, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);
  print_wakeup_reason();
	readSensor();

	if (!btStarted() && !btStart()) {
		ESP_LOGD(TAG, "BT Start Fail");
	}
	esp_bluedroid_status_t bt_state = esp_bluedroid_get_status();
	if (bt_state == ESP_BLUEDROID_STATUS_UNINITIALIZED) {
		if (esp_bluedroid_init()) {
			ESP_LOGD(TAG, "init failed");
		}
	}
	if (bt_state != ESP_BLUEDROID_STATUS_ENABLED) {
		if (esp_bluedroid_enable()) {
			ESP_LOGD(TAG, "enable failed");
		}
	}
	//D000 0000CDCC A441 F7030000 EC
	char *hex = barray2hexstr(sensor.bytes, 14);
	ESP_LOGD(TAG, "%s", hex);
	delete[] hex;
	esp_ble_gap_set_device_name("Oxygen Meter");
	adv_config.p_manufacturer_data = sensor.bytes;
	adv_config.manufacturer_len = 13;
	esp_ble_gap_config_adv_data(&adv_config);
	esp_ble_gap_register_callback(gap_handler);
	bootcount++;
	ESP_LOGD(TAG, "Advertizing started...");
	delay(100);
	ESP_LOGD(TAG, "enter deep sleep");
	esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
	esp_deep_sleep_start();
}

void loop() {
}