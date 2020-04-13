#ifndef __LCD_HPP__
#define __LCD_HPP__

#include "config.hpp"
#include "type.hpp"
#include <Arduino.h>
#include <U8g2lib.h>
#include "esp_bt_device.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

RTC_DATA_ATTR device_status_t status;

class LCD {
private:
	U8G2_SSD1327_WS_128X128_F_4W_HW_SPI *u8g2;
	bool alreadyBegin;
public:
	LCD(U8G2_SSD1327_WS_128X128_F_4W_HW_SPI *_u8g2) 
        : u8g2(_u8g2), alreadyBegin(false) {
			
	}
	void begin() {
		if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_UNDEFINED) {
            this->u8g2->beginSimple();
            this->u8g2->setPowerSave(0);
		} else {
            this->u8g2->begin();
		}
		alreadyBegin = true;
	}

	void clear(){
		if(!alreadyBegin){
			begin();
		}
		this->u8g2->clear();
	}

	void print() {
		if(!alreadyBegin){
			begin();
		}
		this->u8g2->setDrawColor(1);
		this->u8g2->setFontMode(1);
		this->u8g2->setFontDirection(0);
		this->u8g2->clearBuffer();
		this->u8g2->setFont(u8g2_font_fub25_tf);
		char str[32] = {
			0,
		};
		sprintf(str, "%.2f%%", status.sensor.o2);
		this->u8g2->drawStr(
			(this->u8g2->getDisplayWidth() - this->u8g2->getStrWidth(str)) >> 1,
			this->u8g2->getDisplayHeight() - (this->u8g2->getMaxCharHeight() >> 1),
			str);

		if(status.sensor.warringO2 | status.sensor.requestSos){
		this->u8g2->setFont(u8g2_font_profont10_tr);
			memset(str, 0x00, sizeof(char) * 32);
			sprintf(str, "%.2f%%", status.sensor.o2);
			const uint8_t *point = esp_bt_dev_get_address();

			char str[32];
			sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X", 
						(int)point[0], (int)point[1], (int)point[2], 
						(int)point[3], (int)point[4], (int)point[5]);
			this->u8g2->drawStr(
				(this->u8g2->getDisplayWidth() - this->u8g2->getStrWidth(str)) >> 1, 120, str);
		}
		this->u8g2->setFont(u8g2_font_open_iconic_all_4x_t);
		this->u8g2->drawGlyph(5, 40, 0x79 - (status.sensor.isOk));
		this->u8g2->sendBuffer();
	}
};
#endif