
#include "ble.hpp"

esp_err_t BLE::broadcast(sensor_t *sensor) {
	if (!btStarted() && !btStart()) {
		ESP_LOGD(TAG, "BT Start Fail");
	}
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
	
	//데이터 설정
	adv_config.p_manufacturer_data = sensor->bytes;
	adv_config.manufacturer_len = 13;

	//BLE 에러 체크 및 설정
	ESP_ERROR_CHECK(esp_ble_gap_set_device_name("Oxygen Meter"));
	ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&adv_config));
	ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_handler));

	return ESP_OK;
}