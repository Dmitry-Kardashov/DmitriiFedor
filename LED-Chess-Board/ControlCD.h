#include <SD.h>
#include <Adafruit_NeoPixel.h>
#include "Configuration.h"
Adafruit_NeoPixel SidematrixLed(35, MatrixLed, NEO_GRB + NEO_KHZ800);  
class CCD {
public:
  CCD(String name);
  void initializingCDcard(uint8_t pin);
private:
  File myFile;
  void CDConfig(bool* status);
  String nameFile;
};