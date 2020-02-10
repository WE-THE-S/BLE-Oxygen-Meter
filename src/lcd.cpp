#include "./lcd.hpp"

void LCD::print(sensor_t *sensor) {
	this->u8g2->clearBuffer();
	this->u8g2->setFont(u8g2_font_profont17_tf);
	char str[32] = {
		0,
	};
	sprintf(str, "o2 : %.2f", sensor->o2);
	this->u8g2->drawStr(0, 20, str);
	memset(str, 0x00, 16);
	sprintf(str, "temp : %.2f", sensor->temp);
	this->u8g2->drawStr(0, 40, str);
	memset(str, 0x00, 16);
	sprintf(str, "ppo2 : %d", sensor->ppO2);
	this->u8g2->drawStr(0, 60, str);
	memset(str, 0x00, 16);
	sprintf(str, "bar : %d", sensor->barometric);
	this->u8g2->drawStr(0, 80, str);
	memset(str, 0x00, 16);
	sprintf(str, "isOk : %d", sensor->isOk);
	this->u8g2->drawStr(0, 100, str);
	this->u8g2->updateDisplay();
}