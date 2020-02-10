#ifndef __LCD_HPP__
#define __LCD_HPP__

#include <Arduino.h>
#include <U8g2lib.h>
#include "./config.hpp"
#include "./type.hpp"

extern RTC_DATA_ATTR U8G2_SSD1327_WS_128X128_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/14, /* dc=*/5, /* reset=*/19);

class LCD {
    public:
        void begin(){
            u8g2.begin();
        }
        void print(sensor_t*);
};
#endif