// общее демо библиотеки. Обязательно смотри остальные примеры и документацию на гитхаб!
// где получить токен и ID https://kit.alexgyver.ru/tutorials/telegram-basic/

#define WIFI_SSID "monstr"
#define WIFI_PASS "monstr123ex"
#define BOT_TOKEN "5744408962:AAGC2OHAClI2HQFrlNzMb1NstD9zVhu4Eoc"
#define CHAT_ID "1182132923"
#define LOCK_PIN D5

#include <FastBot.h>
#include <Adafruit_Fingerprint.h>


#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(D6, D7);
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
FastBot bot(BOT_TOKEN);

void setup() {
  Serial.begin(9600);
   while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");
  connectWiFi();
  bot.setChatID(CHAT_ID); // передай "" (пустую строку) чтобы отключить проверку
  bot.attach(newMsg);
  bot.sendMessage("Hello, World!");
  pinMode(LOCK_PIN,OUTPUT);
  digitalWrite(LOCK_PIN,HIGH);
    finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
}

// обработчик сообщений
void newMsg(FB_msg& msg) {
  // выводим имя юзера и текст сообщения
  Serial.print(msg.username);
  Serial.print(", ");
  Serial.println(msg.text);
  if (msg.text == "/open")
  {
    digitalWrite(LOCK_PIN, LOW);
    bot.sendMessage("Дверь разблокирована!", msg.chatID);
    delay(3000);
    digitalWrite(LOCK_PIN,HIGH);
    bot.sendMessage("Дверь заблокирована!", msg.chatID);
  }
  

}

void loop() {
  bot.tick();   // тикаем в луп
  getFingerprintID();
}

void connectWiFi() {
  delay(2000);
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() > 15000) ESP.restart();
  }
  Serial.println("Connected");
}
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

 if(finger.fingerID == 1 || finger.fingerID == 2) 
 {
  Serial.println("Open");
  digitalWrite(LOCK_PIN, LOW);
  delay(3000);
  digitalWrite(LOCK_PIN,HIGH);
  }

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

if(finger.fingerID == 1 || finger.fingerID == 2) 
 {
  Serial.println("Open");
  digitalWrite(LOCK_PIN, LOW);
  delay(3000);
  digitalWrite(LOCK_PIN,HIGH);
  }
  return finger.fingerID;
 
}