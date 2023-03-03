// изменение имени окна

#define AP_SSID "monstr"
#define AP_PASS "monstr123ex"

#include <GyverPortal.h>
GyverPortal ui;

void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  // однократно изменить название окна
  //GP.PAGE_TITLE("MyPortal");

  // изменить и подключить обновление (1 в сек)
  GP.PAGE_TITLE("MyPortal", "tit");
  GP.UPDATE("tit");
  
  GP.BUILD_END();
}

void action() {
  // менять при обновлении
  if (ui.update("tit")) {
    ui.answer(String("teplica");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  ui.attachBuild(build);
  ui.attach(action);
  ui.start();
}

void loop() {
  ui.tick();
}
