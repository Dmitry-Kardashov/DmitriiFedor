const int PIN_CHIP_S_SD = 7;
//-------НАСТРОЙКИ---------

String fString, fStringFileName, rwBuff;
char logfile[] = {"testLase.nc"};


//-------БИБЛИОТЕКИ---------
String buffer;//тут хранится прочитанный из карты текст
boolean FileOpen, FileClose, FileReady;

#include <SPI.h>
#include <SD.h>
File file;

//File dataFile;
//-------БИБЛИОТЕКИ---------

void initializeSD() {
  Serial.println("Initializing SD card...");
  pinMode(PIN_CHIP_S_SD, OUTPUT);
  if (SD.begin()) {
    Serial.println("SD card is ready to use.");
  } else {
    Serial.println("SD card initialization failed");
    return;
  }
}

void closeFile() {
  if (file) {
    file.close();
    Serial.println("File closed");
  }
}

int openFile(char filename[]) {
  file = SD.open(filename);
  if (file) {
    Serial.println("File opened with success!");
    return 1;
  } else {
    Serial.println("Error opening file...");
    return 0;
  }
}
String readLine() {
  String received = "";
  char ch;
  while (file.available()) {
    ch = file.read();
    if (ch == '\n') {
      return String(received);
    } else {
      received += ch;
    }
  }
  return "";
}

void FileRead() {
  if (FileOpen != 1) {
    initializeSD();
    openFile(logfile);
    FileOpen = 1;
  }
  if (file.available() > 0) {
    Serial.println(readLine());
  }
  else FileClose = 1;

  if (FileClose == 1) {
    closeFile();
    FileClose = 0;
    FileOpen = 0;
    //    Serial.println("File Close");
  }
}
void setup() {
  Serial.begin(115200);                   // открыть порт для связи с ПК для отладки
}


void loop() {
  FileRead();
  for (int i = 0; i < 6; i++) {
    Serial.print(i);
    delay(200);
  }
  Serial.println("");
}