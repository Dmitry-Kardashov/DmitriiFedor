#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "DHT.h" 

#define DHTPIN D2  
#define DHTTYPE DHT22  

DHT dht(DHTPIN, DHTTYPE);
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
int temperature, humidity, pressure, altitude;
/* Введите SSID и пароль от вашей сети */
const char* ssid = "monstr";   // SSID
const char* password = "monstr123ex";  // пароль

ESP8266WebServer server(80);        


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Wifi network station credentials
#define WIFI_SSID "monstr"
#define WIFI_PASSWORD "monstr123ex"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "5334608653:AAFvubZ9YByK9pa6Ee1A0G6qT0VWk5PeUyY"

String chat_id =        "1182132923";

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

void handleNewMessages(int numNewMessages)
{
  for (int i = 0; i < numNewMessages; i++)
  {
    String text = bot.messages[i].text;
    if(text == "/start")
  { 
      String StartT = "Добрый день. Вывожу список доступных команд: \n";
      StartT += "/temp_in - Температура DHT \n";
      //StartT += "/temp_land - Температура почвы \n";
      StartT += "/hum_in - Влажность DHT \n";
     // StartT += "/hum_land - Влажность почвы \n";
      bot.sendMessage(chat_id, StartT, "");
  }
}}
 
 
void setup() 
{
  dht.begin(); // запускаем датчик
  Serial.begin(115200);
  delay(100);
  
  bme.begin(0x76);   

  Serial.println("Connecting to ");
  Serial.println(ssid);

  // подключиться к вашей локальной wi-fi сети
  WiFi.begin(ssid, password);
secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  // проверить, выполнено ли подключение wi-fi сети
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
  
  

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");



 time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

}
void loop() 
{
  
   temperature = dht.readHumidity();
  humidity = dht.readTemperature();
  server.handleClient();
 if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }

}

void handle_OnConnect() 
{
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  server.send(200, "text/html", SendHTML(temperature,humidity,pressure,altitude)); 
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}

String SendHTML(int temperature, int humidity,int pressure,int altitude)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 Weather Station</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP8266 Weather Station</h1>\n";
  ptr +="<p>Temperature: ";
  ptr +=temperature;
  ptr +="&deg;C</p>";
  ptr +="<p>Humidity: ";
  ptr +=humidity;
  ptr +="%</p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
