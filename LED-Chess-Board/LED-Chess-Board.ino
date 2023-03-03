#include "Configuration.h"
#include "ControlWifi.h"
#include "ControlCD.h"
#include "Backlight.h"
//#include <Adafruit_NeoPixel.h>

//Adafruit_NeoPixel SidematrixLed(35, MatrixLed, NEO_GRB + NEO_KHZ800);

/*
Эта ветка показывает структуру проекта!
*/

void setup() {
  SidematrixLed.begin();                     // инициализируем объект NeoPixel
  SidematrixLed.clear(); // Set all pixel colors to 'off'
  initilization();   // Подготовка к запуску
  connectedWifi();   // Подключение к wifi
  updateFirmware();  // Запуск прошивки по wifi
}

void loop() {
  updateServer();
}

void initilization() {
  CCD cdcard(CONFFILE);
  cdcard.initializingCDcard(CDPIN);
 
     Backlight().initBacklight();
  
}

void connectedWifi() {
  CWIFI controlWifi(APSSID, APPSK);  // Пробуем подключиться к первой сети
  if (!controlWifi.connectWiFi(TIMEOUT, INDICATOR_OF_NETWORK_STATUS)) {
    CWIFI controlWifi(APSSID1, APPSK1);  // Пробуем подключиться ко второй сети
    if (!controlWifi.connectWiFi(TIMEOUT, INDICATOR_OF_NETWORK_STATUS)) {
      CWIFI controlWifi(APSSID2, APPSK2);  // Пробуем подключиться к третьей сети
      controlWifi.connectWiFi(TIMEOUT, INDICATOR_OF_NETWORK_STATUS);
    }
  }
  //костыль можно оптимизировать(много одинаковых операций)
}
