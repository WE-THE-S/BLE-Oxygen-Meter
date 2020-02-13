
#include "ble.hpp"

esp_err_t BLE::broadcast(sensor_t *sensor) {
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
	esp_ble_gap_set_device_name("Oxygen Meter");
	adv_config.p_manufacturer_data = sensor->bytes;
	adv_config.manufacturer_len = 13;
	esp_ble_gap_config_adv_data(&adv_config);
	esp_ble_gap_register_callback(gap_handler);
	return ESP_OK;
}