#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <MyHomeBridgeWifi.h>
#include <MyWifiSettings.h>
#include "FS.h"

MyHomeBridgeWifi wifi;

ESP8266WebServer server(80);

unsigned char _dual_status = 0;

uint8_t       power = 0;

void relayStatusBis(uint8_t id, uint8_t state) {
    uint8_t mask = 0x01 << 0 << id;
    if (!state) {
      power &= (0xFF ^ mask);
    }
    else if (state == 1) {
      power |= mask;
    }    
    else {
      power ^= mask;
    }
    Serial.write(0xA0);
    Serial.write(0x04);
    Serial.write(power);
    Serial.write(0xA1);
    Serial.write('\n');
    Serial.flush();
}

void setup()
{
  Serial.begin(19200);

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
  }  

  IPAddress ip(192,168,0,99);
  wifi.init(MYWIFISSID, MYWIFIPASSWORD, ip);
  wifi.connect();

  initController();
  server.begin();
  Serial.println("HTTP server started");    
}

void loop()
{
  server.handleClient();
  wifi.check();
}

