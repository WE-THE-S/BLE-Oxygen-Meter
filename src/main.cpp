#include <Arduino.h>
#include <cstring>
#include <cstdlib>
#include <HardwareSerial.cpp>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

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


class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    BLEDevice::startAdvertising();
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, SENSOR_RX_PIN, SENSOR_TX_PIN);

  BLEDevice::init("Oxygen Meter");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0); 
  BLEDevice::startAdvertising();
  ESP_LOGI("Waiting a client connection to notify...");
}

void loop() {
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
        volatile double* value = nullptr;
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
    if (deviceConnected) {
        pCharacteristic->setValue((uint8_t*)&value, 4);
        pCharacteristic->notify();
        value++;
    }
    
    if (!deviceConnected && oldDeviceConnected) {
        delay(500);
        pServer->startAdvertising(); 
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}