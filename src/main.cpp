#include <Arduino.h>
#include <cstring>
#include <cstdlib>

const int SENSOR_TX_PIN = 20;
const int SENSOR_RX_PIN = 21;

typedef struct {
  double ppO2;
  double temp;
  double barometric;
  double o2;
  bool isOk;
} sensor_t;

volatile sensor_t sensor;

void IRAM_ATTR SerialEvent2(){
  char temp[256] = {0, };
  Serial2.readBytesUntil('\n', temp, 256);
  char* token = strtok(temp, " ");
  while(token != NULL){
    char command = token[0];
    token = strtok(NULL, " ");
    if(command == 'e'){
      int code = atoi(token);
      sensor.isOk = (code == 0);
    }else{
      double* value;
      switch(command){
        case 'O' : {
          value = &(sensor.ppO2);
          break;
        }
        case 'T' : {
          value = &(sensor.temp);
          break;
        }
        case 'P' : {
          value = &(sensor.barometric);
          break;
        }
        case '%' : {
          value = &(sensor.o2);
          break;
        }
      }
      *value = atof(token);
    }
  }
}

void setup() {
  Serial2.begin(9600, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);
  Serial2.setInterrupt(&SerialEvent2);
}

void loop() {
  
}