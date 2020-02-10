#ifndef __LCD_HPP__
#define __LCD_HPP__

#include <Arduino.h>
#include <U8g2lib.h>
#include "config.hpp"
#include "type.hpp"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

class LCD {
    private:
        U8G2_SSD1327_WS_128X128_F_4W_HW_SPI* u8g2;
    public:
        LCD(U8G2_SSD1327_WS_128X128_F_4W_HW_SPI* _u8g2) : u8g2(_u8g2){

        }
        void begin(){
            this->u8g2->begin();
        }
        void print(sensor_t*);
};
#endif