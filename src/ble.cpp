
#include "./ble.hpp"

esp_err_t BLE::broadcast() {
	if (!btStarted() && !btStart()) {
		ESP_LOGD(TAG, "BT Start Fail");
		return ESP_FAIL;
	}
	ESP_LOGD(TAG, "BT Start");
	ESP_ERROR_CHECK(esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_N12));
	ESP_ERROR_CHECK(esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9));
	ESP_LOGD(TAG, "Advertising Init");
	switch(esp_bluedroid_get_status()){
		case ESP_BLUEDROID_STATUS_ENABLED : {
			break;
		}
		case ESP_BLUEDROID_STATUS_UNINITIALIZED : 
			ESP_ERROR_CHECK(esp_bluedroid_init());
		default : {
			ESP_ERROR_CHECK(esp_bluedroid_enable());
			break;
		}
	}
	ESP_LOGD(TAG, "BT Enable");
	return ESP_OK;
}


esp_err_t BLE::update(sensor_t *sensor) {
	ESP_LOGD(TAG, "BT Update");
	//데이터 설정
	adv_config.p_manufacturer_data = sensor->bytes;
	adv_config.manufacturer_len = 13;

	//BLE 에러 체크 및 설정
	ESP_ERROR_CHECK(esp_ble_gap_set_device_name("Oxygen Meter"));
	ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&adv_config));
	ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_handler));

	return ESP_OK;
}

