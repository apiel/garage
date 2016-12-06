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

int relayState = LOW; // Off
int powerState = LOW; // Off

int powerOnIn = -1;
int powerOffIn = -1;

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

void powerOff() {
  _logln("Power off");
  turnOff();
  digitalWrite(SONOFF_LED, LOW);
  powerState = LOW;  
}

void powerOn() {
  _logln("Power on");
  ticker.attach(2, tick);
  powerState = HIGH;  
}

void powerToggle() {
  _logln("toggle power state");
  _logln("" + powerState);
  if (powerIsOn()) {
    powerOff();
  }
  else {
    powerOn();
  }
}

void powerHandle() {
  if (powerOnIn > -1 && powerOnIn < millis()) {
    powerOnIn = -1;
    powerOn();
  }
  if (powerOffIn > -1 && powerOffIn < millis()) {
    powerOffIn = -1;
    powerOff();
  }
}

void relayTemperatureToggle() {
  if (powerIsOn()) {
    sensors.requestTemperatures();
    //float t = sensors.getTempCByIndex(0);
    float t = readFile("mockTemp", "21").toFloat();
    setRelayInFunctionOfTemperature(t);
  }
}

void setRelayInFunctionOfTemperature(float t) {
    if (!isnan(t) && t > -127) {
      if (t < onTemperature && !relayIsOn()) {
        _logln("Turn on");
        turnOn();
      }
      else if (t > offTemperature && relayIsOn()) {
        _logln("Turn off");
        turnOff();
      } 
    }
    else {
      _logln("Temperature invalid, turn off");
      turnOff();
    }
}

void setup()
{
  Serial.begin(9600);

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
  
  Serial.println("done setup");

  sensors.begin();

  IPAddress ip(192,168,0,85);
  wifi.init(MYWIFISSID, MYWIFIPASSWORD, ip);
  wifi.connect();

  initController();
  server.begin();
  initLogServer();
  Serial.println("HTTP server started");    
}

void loop()
{
  server.handleClient();
  sonoffButtonTrigger();
  relayTemperatureToggle();
  wifi.check();
  handleLogServer();
  powerHandle();
}

