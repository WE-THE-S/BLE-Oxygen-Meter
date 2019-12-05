#ifndef __OXYGEN_HPP__
#define __OXYGEN_HPP__
/***
 * ESP32 용 산소 센서 라이브러리
 */
#include <Arduino.h>
#include <inttypes.h>
#include <stdbool.h>
#include <vector>
#include <string>

using namespace std;

typedef enum {
    STREAM = 0,
    POLL = 1,
    OFF = 2,

    DATE_OF_MANUFACTURE = STREAM,
    SERIAL_NUMBER = POLL,
    SOFTWARE_REVISION = OFF
} OxygenOutputMode_t;

typedef enum {
    REQUEST_PPO2 = 'O',
    REQUEST_O2 = '%',
    REQUEST_TEMPERATURE = 'T',
    REQUEST_BAROMETRIC = 'P',
} OxygenSensorRequest_t;

typedef enum {
    SET_OUTPUT_MODE = 'M',
    REQUEST_ALL = 'A',
    SENSOR_STATUS = 'e',
    SENSOR_INFO = '#'
} OxygenRequest_t;

typedef struct {
    uint8_t* data;
    uint32_t len;
} OxygenData_t;
class Oxygen{
    private: 
        OxygenData_t request(OxygenData_t packet);
        OxygenData_t request(uint8_t command);
        OxygenData_t request(uint8_t command, uint8_t arg);
    public:
        Oxygen(int rx, int tx){
            Serial2.begin(9600, SERIAL_8N1, rx, tx);
        }
        double sensorValue(OxygenSensorRequest_t sensor);
        double* allSensorValue();
        bool sensorStatus();
        void setOutputMode(OxygenOutputMode_t type);
        string sensorInfo(OxygenOutputMode_t type);
};

#endif