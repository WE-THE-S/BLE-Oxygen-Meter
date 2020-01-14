#include <Arduino.h>
#include <cstring>
#include <cstdlib>
#include <HardwareSerial.cpp>
#include <esp_log.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define uS_TO_S_FACTOR 1000 * 1000
#define TIME_TO_SLEEP  10

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const int SENSOR_TX_PIN = 35;
const int SENSOR_RX_PIN = 32;

typedef union {
  struct{
    float ppO2;
    float temp;
    float barometric;
    float o2;
    bool isOk;
  };
  uint8_t bytes[13];
} sensor_t;

volatile sensor_t sensor;

void readSensor(){
  char temp[256] = {0, };
  Serial2.setTimeout(2000);
  Serial2.readBytesUntil('\n', temp, 256);
  char* token = strtok(temp, " ");
  while(token != NULL){
    ESP_LOGD("raw", "%s", token);
    char command = token[0];
    
    if(command == 'e'){
      int code = atoi(token);
      sensor.isOk = (code == 0);
    }else{
      volatile float* value = nullptr;
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
      if(value != nullptr){
        *value = atof(token);
      }
    }
  }
}

void setup() {
  sensor.isOk = false;
  u8g2.begin();
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);
  readSensor();
  BLEDevice::init("Oxygen Meter");
  BLEServer* pServer = BLEDevice::createServer();
  BLEService* pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic* pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ
                      );

  pCharacteristic->setValue(const_cast<uint8_t*>(sensor.bytes), sizeof(sensor_t));
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0);
   
  BLEDevice::startAdvertising();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  ESP_LOGI("Notify", "Waiting a client connection to notify...");
  esp_deep_sleep_start();
}

void loop() {
}