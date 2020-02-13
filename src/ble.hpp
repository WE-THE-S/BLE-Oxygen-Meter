#ifndef __BLE_HPP__
#define __BLE_HPP__

#include "./config.hpp"
#include "./type.hpp"
#include "driver/adc.h"
#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_wifi.h"

static esp_ble_adv_data_t adv_config = {
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

static esp_ble_adv_params_t adv_param = {
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
class BLE {
private:
	//9A99D541 B81E9341 01 00B2 00F7
	const char *TAG = "BLE";

public:
	void begin() {
		adc_power_off();
		esp_wifi_deinit();
		esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_N12);
		esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
		ESP_LOGD(TAG, "Advertising Init");
	}
	esp_err_t broadcast(sensor_t *sensor);
};

#endif