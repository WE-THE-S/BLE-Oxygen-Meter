#include "./lcd.hpp"

RTC_DATA_ATTR device_status_t status;

void LCD::print() {
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
	
	this->u8g2->setFont(u8g2_font_open_iconic_all_4x_t);
	this->u8g2->drawGlyph(5, 40, 0x79 - (status.sensor.isOk));
	this->u8g2->sendBuffer();
}