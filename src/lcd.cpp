#include "./lcd.hpp"

void LCD::print() {
	this->u8g2->clearBuffer();
	this->u8g2->setFont(u8g2_font_fub25_tf);
	char str[32] = {
		0,
	};
	switch (this->status->menu) {
		case O2: {
			sprintf(str, "%.2f%%", this->status->sensor.o2);
			break;
		}
		case PPO2 : {
			sprintf(str, "%d", this->status->sensor.ppO2);
			break;
		}
		case BAR : {
			sprintf(str, "%d", this->status->sensor.barometric);
			break;
		}
		case TEMP : {
			sprintf(str, "%.2f%%", this->status->sensor.temp);
			break;
		}
		case BATTERY : {
			sprintf(str, "%.2f%%", this->status->sensor.temp);
			break;
		}
		default: {
			sprintf(str, "test");
			break;
		}
	}
	this->u8g2->drawStr(
		(this->u8g2->getDisplayWidth() - this->u8g2->getStrWidth(str)) >> 1,
		128 - (this->u8g2->getMaxCharHeight() >> 1),
		str);
	
	this->u8g2->setFont(u8g2_font_open_iconic_all_4x_t);
	this->u8g2->drawGlyph(5, 40, 0x79 - (this->status->sensor.isOk));
	this->u8g2->updateDisplay();
	while(this->u8g2->nextPage());
}