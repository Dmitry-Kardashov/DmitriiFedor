#include "ControlCD.h"
#include <SPI.h>
#include "Backlight.h"
#include "Configuration.h"



CCD::CCD(String name) {  //конструктор класса
  nameFile = name;
}



void CCD::CDConfig(bool* status) {  //функция проверки наличия config.txt
  if (*status) {
    myFile = SD.open(nameFile);
    *status *= bool(myFile);
  }
}

void CCD::initializingCDcard(uint8_t pin) {  //функция проверки cd карты на её наличие
  bool statusCD = SD.begin(pin);
   if(statusCD)
  {
    SidematrixLed.setPixelColor(1, SidematrixLed.Color(0, 250, 0));  // указываем зелёный цвет для первого пикселя
    SidematrixLed.setPixelColor(2, SidematrixLed.Color(0, 0, 0));    // отключаем красный цвет для второрго пикселя
    
  }
  else
  {
    SidematrixLed.setPixelColor(1, SidematrixLed.Color(0, 0, 0));  // отлючаем  зелёный цвет для первого пикселя
    SidematrixLed.setPixelColor(2, SidematrixLed.Color(250, 0, 0));    // указываем красный цвет для второго пикселя
  }
    CDConfig(&statusCD); //отправляем в функцию указатель на переменную, что делает возможным изменения этой переменной внутри функции
}

//по итогу выполнения этих двух функций в statusCD будет информация о существовании старой конфигурации доски