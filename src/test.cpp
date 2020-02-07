#include <Arduino.h>
#include <driver/rtc_io.h>
#include <driver/uart.h>
#include <esp_sleep.h>
#include <soc/uart_channel.h>

#define uS_TO_S_FACTOR 1000000  //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  5        //Time ESP32 will go to sleep (in seconds)

typedef union {
  struct {
    float temp;
    float o2;
    int16_t ppO2;
    int16_t barometric;
    bool isOk;
  };
  uint8_t bytes[];
} sensor_t;

RTC_DATA_ATTR sensor_t sensor;
RTC_DATA_ATTR int wake_count;

void print_wakeup_reason(){
  rtc_gpio_hold_dis(GPIO_NUM_32);
  rtc_gpio_init(GPIO_NUM_32);
  rtc_gpio_set_direction(GPIO_NUM_32, RTC_GPIO_MODE_OUTPUT_ONLY);
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0  : {
      rtc_gpio_set_level(GPIO_NUM_32, HIGH);
      wake_count++;
      Serial.print(wake_count);
      Serial.println("Wakeup caused by external signal using RTC_IO"); 
      delay(100);
      break;
    }
    case ESP_SLEEP_WAKEUP_EXT1  : {
      Serial.println("Wakeup caused by external signal using RTC_CNTL"); 
      delay(100);
      break;
    }
    case ESP_SLEEP_WAKEUP_TIMER : {
      Serial.println("Wakeup caused by timer"); 
      rtc_gpio_set_level(GPIO_NUM_32, LOW);
      readSensor();
      wake_count = 0;
      break;
    }
    case ESP_SLEEP_WAKEUP_TOUCHPAD  : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP  : Serial.println("Wakeup caused by ULP program"); break;
    default : {
      Serial.println("Wakeup was not caused by deep sleep"); 
      rtc_gpio_set_level(GPIO_NUM_32, LOW);
      wake_count = 0;
      break;
    }
  }
  gpio_hold_en(GPIO_NUM_32);
}

void readSensor() {
  const char *TAG = "Sensor";
  String recv = Serial2.readStringUntil('\n');
  if(recv.length() > 5){
    recv.toLowerCase();
    recv = recv.substring(recv.indexOf('o'));
    char *temp = const_cast<char *>(recv.c_str());
    Serial.println(recv);
    char *token = strtok(temp, " ");
    while (token != NULL) {
      //ESP_LOGD(TAG, "%s", token);
      char command = token[0];
      token = strtok(NULL, " ");
      switch (command) {
        case 'e': {
          int code = atoi(token);
          sensor.isOk = (code == 0);
          break;
        }
        case 'o': {
          sensor.ppO2 = static_cast<int16_t>(atof(token));
          break;
        }
        case 't': {
          sensor.temp = atof(token);
          break;
        }
        case 'p': {
          float temp = atof(token);
          sensor.barometric = static_cast<int16_t>(temp);
          break;
        }
        case '%': {
          sensor.o2 = atof(token);
          break;
        }
      }
    }
  }
}

void loop2(void *pvParameters) {
  Serial.println(xPortGetCoreID());
  while (1) {
     int ctrl = digitalRead(34);
     int io = digitalRead(33);
     //Serial.print(io);
     //Serial.print(", ");
     //Serial.println(ctrl);
     
  }
}

void loop3(void *pvParameters) {
  Serial.println(xPortGetCoreID());
  print_wakeup_reason(); 
  ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR));
  ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1));
  ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT64(GPIO_NUM_34), ESP_EXT1_WAKEUP_ANY_HIGH));
  
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 35, 32);
  Serial2.setTimeout(500);
  delay(1000);
  xTaskCreatePinnedToCore(loop2, "loop2", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(loop3, "loop3", 4096, NULL, 1, NULL, 1);
}

void loop() {
  // put your main code here, to run repeatedly:

}