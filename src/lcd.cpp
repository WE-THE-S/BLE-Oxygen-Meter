#include "./lcd.hpp"

void LCD::print() {
	this->u8g2->clearBuffer();
	this->u8g2->setFont(u8g2_font_profont17_tf);
	char str[32] = {
		0,
	};
	sprintf(str, "o2 : %.2f", this->status->sensor.o2);
	this->u8g2->drawStr(0, 20, str);
	memset(str, 0x00, 32);
	sprintf(str, "temp : %.2f", this->status->sensor.temp);
	this->u8g2->drawStr(0, 40, str);
	memset(str, 0x00, 32);
	sprintf(str, "ppo2 : %d", this->status->sensor.ppO2);
	this->u8g2->drawStr(0, 60, str);
	memset(str, 0x00, 32);
	sprintf(str, "bar : %d", this->status->sensor.barometric);
	this->u8g2->drawStr(0, 80, str);
	memset(str, 0x00, 32);
	sprintf(str, "isOk : %d", this->status->sensor.isOk);
	this->u8g2->drawStr(0, 100, str);
	this->u8g2->updateDisplay();
}