/*******************************************************************
    A telegram bot for your ESP8266 that controls the 
    onboard LED. The LED in this example is active low.

    Parts:
    D1 Mini ESP8266 * - http://s.click.aliexpress.com/e/uzFUnIe
    (or any ESP8266 board)

      = Affilate

    If you find what I do useful and would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/


    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include "DHT.h" 
#include <GyverNTC.h>

// Wifi network station credentials
#define WIFI_SSID "Coworking"
#define WIFI_PASSWORD "Kvant1234"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "5334608653:AAFvubZ9YByK9pa6Ee1A0G6qT0VWk5PeUyY"
GyverNTC therm(0, 10000, 3950);

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

const int ledPin = LED_BUILTIN;
int ledStatus = 0;
ESP8266WebServer server(80); 

#define DHTPIN D3  
#define DHTTYPE DHT22  

DHT dht(DHTPIN, DHTTYPE);
float temperature, humidity, pressure, altitude;
//IPAddress ip = WiFi.localIP();

void handleNewMessages(int numNewMessages)
{
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/ledon")
    {
      digitalWrite(ledPin, LOW); // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
    }

    if (text == "/ledoff")
    {
      ledStatus = 0;
      digitalWrite(ledPin, HIGH); // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
    }

    if (text == "/status")
    {
      if (ledStatus)
      {
        bot.sendMessage(chat_id, "Led is ON", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }

    if(text == "/start")
  { 
      String StartT = "Добрый день. Вывожу список доступных команд: \n";
      StartT += "/temp_in - Температура DHT \n";
      StartT += "/temp_out - Температура термистора \n";
      StartT += "/hum_in - Влажность DHT \n";
      StartT += "/get_ip - Текущий ip устройства \n";
      bot.sendMessage(chat_id, StartT, "");
  }
  if(text == "/temp_in")
  {
    String temp = "Температура DHT = \n";
    temp += t;  
    temp += "C \n";    
    bot.sendMessage(chat_id, temp, "");
  }

   if(text == "/hum_in")
  {
    String hum = "Влажность DHT = \n";
    hum += h;  
    hum += "% \n";    
    bot.sendMessage(chat_id, hum, "");
  }
   if(text == "/temp_out")
  {
    String t1 = "Температура термистора = \n";
    t1 += therm.getTempAverage();  
    t1 += "C \n";    
    bot.sendMessage(chat_id, t1, "");
  }/*
if(text == "/get_ip")
  {
    String getip = "Текущий ip устройства - \n";
    getip += ip;  
    //temp += "% \n";    
    bot.sendMessage(chat_id, getip, "");
  }  */
  }
}


void setup()
{

    Serial.println(therm.getTempAverage());
    Serial.begin(115200);
  Serial.println();
dht.begin(); // запускаем датчик
  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, HIGH); // initialize pin as off (active LOW)

  // attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // Check NTP/Time, usually it is instantaneous and you can delete the code below.
  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);  
  server.begin();
}

void loop()
{
   temperature = dht.readTemperature();
  humidity = dht.readHumidity();
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
  //pressure = bme.readPressure() / 100.0F;
  //altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  server.send(200, "text/html", SendHTML(temperature,humidity, pressure, altitude)); 
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
  ptr +="<p>DHT Temperature: ";
  ptr +=temperature;
  ptr +="&deg;C</p>";
  ptr +="<p>Termistor Temperature: ";
  ptr +=therm.getTempAverage();
  ptr +="&deg;C</p>";
  ptr +="<p>DHT Humidity: ";
  ptr +=humidity;
  ptr +="%</p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
