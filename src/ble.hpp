#ifndef __BLE_HPP__
#define __BLE_HPP__

#include "./config.hpp"
#include "./type.hpp"
#include "driver/adc.h"
#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include <soc/rtc_cntl_reg.h>
#include "esp_wifi.h"

static esp_ble_adv_data_t adv_config = {
	.set_scan_rsp = false,
	.include_name = true,
	.include_txpower = true,
	.min_interval = 0x0020,
	.max_interval = 0x0025,
	.appearance = 0,
	.manufacturer_len = 0,
	.p_manufacturer_data = NULL,
	.service_data_len = 0,
	.p_service_data = NULL,
	.service_uuid_len = 0,
	.p_service_uuid = NULL,
	.flag = (ESP_BLE_ADV_FLAG_NON_LIMIT_DISC)};

static esp_ble_adv_params_t adv_param = {
	.adv_int_min = 0x0020,
	.adv_int_max = 0x0025,
	.adv_type = ADV_TYPE_NONCONN_IND, // Excelent description of this parameter here: https://www.esp32.com/viewtopic.php?t=2267
	.own_addr_type = BLE_ADDR_TYPE_PUBLIC,
	.peer_addr = {
		0x00,
	},
	.peer_addr_type = BLE_ADDR_TYPE_PUBLIC,
	.channel_map = ADV_CHNL_ALL,
	.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};
static void gap_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
	switch (event) {
		case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT: {
			const uint8_t *point = esp_bt_dev_get_address();
			char str[32];
			sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", 
						(int)point[0], (int)point[1], (int)point[2], 
						(int)point[3], (int)point[4], (int)point[5]);
			ESP_LOGI("BT Address", "%s", str);
			ESP_LOGD(TAG, "ADV Update");
			ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&adv_param));
			break;
		}
		default:
			break;
	}
}//30:AE:A4:C6:F4:6A
class BLE {
private:
	//9A99D541 B81E9341 01 00B2 00F7
	const char *TAG = "BLE";
	bool alreadyInit;

	esp_err_t setName(){
		char str[32] = {0, };
		sprintf(str, "Oxygen Meter(%04hX)", status.ssid);
		ESP_LOGI(TAG, "SSID : %s", str);
		return esp_ble_gap_set_device_name(str);
		
	}
public:
	BLE() : alreadyInit(false) {
	}
	
	void begin() {
		WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
	}
	
	esp_err_t broadcast() {
		WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
		if (!alreadyInit) {
			if (btStarted()) {
				stop();
			}
			if (!btStart()) {
				ESP_LOGD(TAG, "BT Start Fail");
				return ESP_FAIL;
			}
			ESP_LOGD(TAG, "BT Start");
			ESP_ERROR_CHECK(esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_N12));
			ESP_ERROR_CHECK(esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_N0));
			ESP_LOGD(TAG, "Advertising Init");
			switch (esp_bluedroid_get_status()) {
				case ESP_BLUEDROID_STATUS_ENABLED: {
					break;
				}
				case ESP_BLUEDROID_STATUS_UNINITIALIZED:
					ESP_ERROR_CHECK(esp_bluedroid_init());
				default: {
					ESP_ERROR_CHECK(esp_bluedroid_enable());
					break;
				}
			}
			ESP_LOGD(TAG, "BT Enable");
			const uint8_t *addr = esp_bt_dev_get_address();
			status.ssid = 0x0;
			for(uint8_t i = 0;i<3;i++){
				uint16_t value = (static_cast<uint16_t>(addr[i]) << 8) + addr[i + 1];
				status.ssid ^= value;
			}
			ESP_ERROR_CHECK(this->setName());
			ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_handler));
			alreadyInit = true;
		}
		return ESP_OK;
	}
	esp_err_t stop() {
		status.checkSendBT = false;
		if (btStarted()) {
			ESP_ERROR_CHECK(esp_bluedroid_disable());
			ESP_ERROR_CHECK(esp_bluedroid_deinit());
			return btStop() ? ESP_OK : ESP_FAIL;
		}
		return ESP_OK;
	}

	esp_err_t update(const sensor_t *sensor) {
		status.checkSendBT = true;
		WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
		ESP_LOGD(TAG, "BT Update");
		//데이터 설정
		auto raw = const_cast<sensor_t*>(sensor);
		adv_config.p_manufacturer_data = raw->bytes;
		adv_config.manufacturer_len = 13;

		//BLE 에러 체크 및 설정
		ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&adv_config));

		return ESP_OK;
	}
};

#endif