#ifndef __OXYGEN_HPP__
#define __OXYGEN_HPP__

#include <Arduino.h>
#include <inttypes.h>

typedef enum {
    STREAM = 0,
    POLL = 1,
    OFF = 2
} OxygenOutputMode_t;
‘
typedef enum {
    SET_OUTPUT_MODE = 'M',
    REQUEST_PPO2 = 'O',
    REQUEST_O2 = '%',
    REQUEST_TEMPERATURE = 'T',
    REQUEST_BAROMETRIC = 'P',
    REQUEST_ALL = 'A',
    SENSOR_STATUS = 'e',
    SENSOR_INFO = '#'
} OxygenRequest_t;

class Oxygen(){
    private: 
        typedef struct {
            uint8_t* data;
            uint32_t len;
        } OxygenData;

        void reqest(uint8_t* data, uint32_t len);
        int recv(uint8_t* data, uint32_t* len);
    public:
        Oxygen::Oxygen(int rx, int tx){
            Serial2.begin(9600, SERIAL_8N1, rx, tx);
            stream = Serial2;
        }

};

#endif