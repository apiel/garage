/*
   1MB flash sizee

   sonoff header
   1 - vcc 3v3
   2 - rx
   3 - tx
   4 - gnd
   5 - gpio 14

   esp8266 connections
   gpio  0 - button
   gpio 12 - relay
   gpio 13 - green led - active low
   gpio 14 - pin 5 on header

*/

#define SONOFF_BUTTON    0
#define SONOFF_RELAY    12
#define SONOFF_LED      13
#define SONOFF_INPUT    14
//#define SONOFF_INPUT    2

#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <MyHomeBridgeWifi.h>
#include <MyWifiSettings.h>
#include "FS.h"

//for LED status
#include <Ticker.h>
Ticker ticker;

MyHomeBridgeWifi wifi;

ESP8266WebServer server(80);

OneWire oneWire(SONOFF_INPUT);
DallasTemperature sensors(&oneWire);

float onTemperature;
float offTemperature;
String temperatureUrl;

int relayState = LOW; // Off
int powerState = LOW; // Off

boolean relayIsOn() {
  return relayState == HIGH;
}

boolean powerIsOn() {
  return powerState == HIGH;
}

float getOnTemperature() {
  return readFile("OnTemperature", "20").toFloat();
}

float getOffTemperature() {
  return readFile("OffTemperature", "22").toFloat();
}

void turnOn() {
  if (powerIsOn() && !relayIsOn()) {
    relayState = HIGH;
    ticker.attach(0.5, tick);
    digitalWrite(SONOFF_RELAY, relayState);
    callUrls(readFile("onUrls", ""));
  }
}

void turnOff() {
  if (powerIsOn() && relayIsOn()) {
    ticker.detach();
    relayState = LOW;
    ticker.attach(2, tick);
    digitalWrite(SONOFF_RELAY, relayState);
    callUrls(readFile("offUrls", ""));  
  }
}

void toggle() {
  Serial.println("toggle power state");
  Serial.println(powerState);
  if (powerIsOn()) {
    turnOff();
    digitalWrite(SONOFF_LED, LOW);
    powerState = LOW;
  }
  else {
    ticker.attach(2, tick);
    powerState = HIGH;
  }
}

/*
long lastCheckForTemperatureUrl = millis();
void temperatureUrlCheck() {
  if (powerIsOn()  && temperatureUrl.length() > 0 && millis() - lastCheckForTemperatureUrl > 3000) { // every minute 60000
    lastCheckForTemperatureUrl = millis();
    Serial.print("Call temperature url: ");
    Serial.println(temperatureUrl);
    HTTPClient http;
    http.begin(temperatureUrl);
    int httpCode = http.GET();
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if(httpCode > 0 && httpCode == HTTP_CODE_OK) {
      String pattern = "{\"status\": "; 
      String payload = http.getString();
      Serial.println(payload);
      if (payload.indexOf(pattern) > -1 ) {
        String t = payload.substring(pattern.length(), payload.indexOf("}"));
        Serial.println("Temperature from remote url: " + t);
        setRelayInFunctionOfTemperature(t.toFloat());
      }
      else {
        Serial.println("Temperature url invalid, turn off relay.");
        turnOff();
      }
    } 
    else {
      Serial.println("Cant temperature url, turn off relay.");
      turnOff();
    }
    http.end(); 
  }
}
*/
void relayTemperatureToggle() {
  if (powerIsOn() && temperatureUrl.length() == 0) {
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(0);
    setRelayInFunctionOfTemperature(t);
  }
}

void setRelayInFunctionOfTemperature(float t) {
    if (!isnan(t) && t > -127) {
      if (t < onTemperature && !relayIsOn()) {
        Serial.println("Turn on");
        turnOn();
      }
      else if (t > offTemperature && relayIsOn()) {
        Serial.println("Turn off");
        turnOff();
      } 
    }
    else {
      Serial.println("Temperature invalid, turn off");
      turnOff();
    }
}

void setup()
{
  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
  }  

  onTemperature = getOnTemperature();
  offTemperature = getOffTemperature();
  
  //set led pin as output
  pinMode(SONOFF_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  //ticker.attach(0.6, tick);

  //ticker.detach();

  initSonoffButton();

  //setup relay
  pinMode(SONOFF_RELAY, OUTPUT);

  turnOff();

  temperatureUrl = readFile("temperatureUrl", "");
  
  Serial.println("done setup");

  sensors.begin();

  IPAddress ip(192,168,0,85);
  wifi.init(MYWIFISSID, MYWIFIPASSWORD, ip);
  wifi.connect();

  initController();
  server.begin();
  Serial.println("HTTP server started");    
}


void loop()
{
  server.handleClient();
  sonoffButtonTrigger();
  relayTemperatureToggle();
  //temperatureUrlCheck();
  wifi.check();
}




