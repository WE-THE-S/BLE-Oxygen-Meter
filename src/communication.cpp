#include "./communication.hpp"

void gap_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
	switch (event) {
		case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT: {
			esp_ble_gap_start_advertising(&adv_param);
			break;
		}
		default:
			break;
	}
}

esp_err_t Communication::broadcast(sensor_t *sensor) {
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

esp_err_t Communication::readSensor(sensor_t *sensor) {
	String recv = Serial2.readStringUntil('\n');
	if (recv.length() > 5) {
		recv.toLowerCase();
		recv = recv.substring(recv.indexOf('o'));
		char *temp = const_cast<char *>(recv.c_str());
		Serial.println(recv);
		char *token = strtok(temp, " ");
		while (token != NULL) {
			//ESP_LOGD(TAG, "%s", token);
			char command = token[0];
			token = strtok(NULL, " ");
			switch (command) {
				case 'e': {
					int code = atoi(token);
					sensor->isOk = (code == 0);
					break;
				}
				case 'o': {
					sensor->ppO2 = static_cast<int16_t>(atof(token));
					break;
				}
				case 't': {
					sensor->temp = atof(token);
					break;
				}
				case 'p': {
					float temp = atof(token);
					sensor->barometric = static_cast<int16_t>(temp);
					break;
				}
				case '%': {
					sensor->o2 = atof(token);
					break;
				}
			}
		}
	} else {
		return ESP_FAIL;
	}
	return ESP_OK;
}