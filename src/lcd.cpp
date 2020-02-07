#include "./lcd.hpp"

void LCD::print(sensor_t sensor){
	u8g2.setFont(u8g2_font_profont17_tf);
	char str[32] = {0, };
	sprintf(str, "o2 : %.2f", sensor.o2);
	u8g2.drawStr(0, 20, str);
	memset(str, 0x00, 16);
	sprintf(str, "temp : %.2f", sensor.temp);
	u8g2.drawStr(0, 40, str);
	memset(str, 0x00, 16);
	sprintf(str, "ppo2 : %d", sensor.ppO2);
	u8g2.drawStr(0, 60, str);
	memset(str, 0x00, 16);
	sprintf(str, "bar : %d", sensor.barometric);
	u8g2.drawStr(0, 80, str);
	memset(str, 0x00, 16);
	sprintf(str, "isOk : %d", sensor.isOk);
	u8g2.drawStr(0, 100, str);
	u8g2.sendBuffer();
}