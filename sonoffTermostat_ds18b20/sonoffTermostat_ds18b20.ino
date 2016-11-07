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
#include "FS.h"

//for LED status
#include <Ticker.h>
Ticker ticker;

#include <MyWifiSettings.h>
const char* wifiSsid = MYWIFISSID;
const char* wifiPassword = MYWIFIPASSWORD;

IPAddress ip(192,168,0,80);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

OneWire oneWire(SONOFF_INPUT);
DallasTemperature sensors(&oneWire);

const int CMD_WAIT = 0;
const int CMD_BUTTON_CHANGE = 1;

int cmd = CMD_WAIT;
int relayState = HIGH;

int powerState = HIGH;

//inverted button state
int buttonState = HIGH;

static long startPress = 0;

float onTemperature;
float offTemperature;

void tick()
{
  int state = digitalRead(SONOFF_LED);  // get the current state of GPIO1 pin
  digitalWrite(SONOFF_LED, !state);     // set pin to the opposite state
}

int callUrl(String url) {
  Serial.print("Call url: ");
  Serial.println(url);
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if(httpCode > 0) {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
  } 
  else {
        Serial.printf("[HTTP] GET... failed, error: %d\n", httpCode);
  }
  http.end(); 

  return httpCode;
}

void callUrls(String urls) {
  int currentPos = 0;
  int pos;
  Serial.println("Call urls: ");
  while ((pos = urls.indexOf('|', currentPos)) > -1) {
    String url = urls.substring(currentPos, pos);
    callUrl(url);
    currentPos = pos + 1;
  }
}

bool saveFile(String file, String data) {
  Serial.print("Save file: ");
  Serial.println(file);

  File configFile = SPIFFS.open(file, "w");
  if (!configFile) {
    Serial.println("Failed to open file for writing");
  }
  else {
    configFile.print(data);
  }
  return configFile;  
}

String readFile(String file, String defaultValue) {
  String value = defaultValue;

  Serial.print("Read file: ");
  Serial.println(file);

  File configFile = SPIFFS.open(file, "r");
  if (!configFile) {
    Serial.println("Failed to open file for reading");
  }
  else {
      value = configFile.readString();       
  }
  return value;
}

float getOnTemperature() {
  return readFile("OnTemperature", "23").toFloat();
}

float getOffTemperature() {
  return readFile("OffTemperature", "25").toFloat();
}

void routeGetOnTemperature() {
  Serial.println("Route GetOnTemperature");
  String response =  "{\"status\": " + String(getOnTemperature()) + "}";
  server.send ( 200, "text/plain", response);
}

void routeGetOffTemperature() {
  Serial.println("Route GetOffTemperature");
  String response =  "{\"status\": " + String(getOffTemperature()) + "}";
  server.send ( 200, "text/plain", response);
}

void routeSetTemperature() {
  Serial.println("routeSetTemperature");
  if (server.hasArg("on") || server.hasArg("off")) {
    if (server.hasArg("on")) {
      String value = server.arg("on");
      offTemperature = value.toFloat();
      saveFile("OnTemperature", value);
    }
    if (server.hasArg("off")) {
      String value = server.arg("off");
      offTemperature = value.toFloat();
      saveFile("OffTemperature", value);
    }    
    server.send ( 200, "text/plain", "Temperature set.");
  }
  else {
    server.send ( 400, "text/plain", "Set OffTemperature parameter missing. Please provide on or/and off.");
  }
}

void routeSetOnUrls() {
  Serial.println("routeSetOnUrls");
  if (server.hasArg("urls")) {
    String value = server.arg("urls");
    saveFile("onUrls", value);    
    Serial.println(value);
    server.send ( 200, "text/plain", "urls on set.");
  }
  else {
    server.send ( 400, "text/plain", "Set on urls parameter missing. Please provide urls.");
  }
}

void routeSetOffUrls() {
  Serial.println("routeSetOffUrls");
  if (server.hasArg("urls")) {
    String value = server.arg("urls");
    saveFile("offUrls", value);    
    Serial.println(value);
    server.send ( 200, "text/plain", "urls off set.");
  }
  else {
    server.send ( 400, "text/plain", "Set off urls parameter missing. Please provide urls.");
  }
}

bool isConnectedToWifi(void) {
  int test = 20;
  while (test && WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  
    test--;
  }
  return test;
}

void connectToWifi() {
  WiFi.disconnect(true);
  Serial.print("Connect to WiFi: ");  
  Serial.println(wifiSsid);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(wifiSsid, wifiPassword);            
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch(event) {
        case WIFI_EVENT_STAMODE_GOT_IP:
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            Serial.println("WiFi lost connection");
            connectToWifi();
            break;
    }
}

void checkWifi() {       
  if (!isConnectedToWifi()) {
      Serial.println("\nDisconnected from Wifi try to reconnect in 5 sec.");
      delay(5000);
      //connectToWifi();
      ESP.reset();
  }  
}

void routeNotFound() {
  Serial.print("Route Not Found ");
  Serial.println(server.uri());
  server.send ( 404, "text/plain", "Not Found!");
}

void routeRoot() {
  Serial.println("Route Root");
  server.send ( 200, "text/plain", "Hello.");
}

void setState(int s) {
  digitalWrite(SONOFF_RELAY, s);
  digitalWrite(SONOFF_LED, (s + 1) % 2); // led is active low
}

boolean isOn() {
  return relayState == HIGH;
}

void routeRelayStatus() {
  Serial.println("Route relay status");
  if (isOn()) {
    server.send ( 200, "text/plain", "{\"status\": \"on\"}");
  }
  else {
    server.send ( 200, "text/plain", "{\"status\": \"off\"}");
  }
}

void turnOn() {
  if (powerState == HIGH) {
    ticker.attach(0.5, tick);
    relayState = HIGH;
    setState(relayState);
    callUrls(readFile("onUrls", ""));
  }
}

void turnOff() {
  if (powerState == HIGH) {
    ticker.detach();
    digitalWrite(SONOFF_LED, LOW);
    relayState = LOW;
    setState(relayState);
    callUrls(readFile("offUrls", ""));  
  }
}

void toggleState() {
  cmd = CMD_BUTTON_CHANGE;
}

void toggle() {
  Serial.println("toggle power state");
  Serial.println(powerState);
  if (powerState == HIGH) {
    turnOff();
    digitalWrite(SONOFF_LED, HIGH);
    powerState = LOW;
  }
  else {
    digitalWrite(SONOFF_LED, LOW);
    powerState = HIGH;
  }
}

void routePowerToggle() {
  toggle();
  routePowerStatus();
}

void routePowerStatus() {
  Serial.println("Route power status");
  if (powerState == HIGH) {
    server.send ( 200, "text/plain", "{\"status\": \"on\"}");  
  }
  else {
    server.send ( 200, "text/plain", "{\"status\": \"off\"}");  
  }
}

void routeRelayOn() {
  turnOn();
  routeRelayStatus();
}

void routeRelayOff() {
  turnOff();
  routeRelayStatus();
}

void restart() {
  ESP.reset();
  delay(1000);
}

void reset() {
  //reset wifi credentials
  WiFi.disconnect();
  delay(1000);
  ESP.reset();
  delay(1000);
}

void routeTemperatureStatus() {
  Serial.println("Route temperature status");
  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);
  if (isnan(t)) {
     server.send ( 200, "text/plain", "{\"status\": \"unknown\"}");
  }
  else {
    String response =  "{\"status\": " + String(t) + "}";
    server.send ( 200, "text/plain", response);
  }
}

void relayTemperatureToggle() {
  if (powerState == HIGH) {
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(0);
    if (!isnan(t)) {
      if (t < onTemperature && !isOn()) {
        Serial.println("Turn on");
        turnOn();
      }
      else if (t > offTemperature && isOn()) {
        Serial.println("Turn off");
        turnOff();
      } 
    }
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

  //setup button
  pinMode(SONOFF_BUTTON, INPUT);
  attachInterrupt(SONOFF_BUTTON, toggleState, CHANGE);

  //setup relay
  pinMode(SONOFF_RELAY, OUTPUT);

  turnOff();
  
  Serial.println("done setup");

  sensors.begin();

  connectToWifi();
  server.on("/", routeRoot);
  server.on("/power/status", routePowerStatus);
  server.on("/power/toggle", routePowerToggle);
  server.on("/get/on/temperature", routeGetOnTemperature);
  server.on("/get/off/temperature", routeGetOffTemperature);
  server.on("/set/temperature", routeSetTemperature);
  server.on("/temperature/status", routeTemperatureStatus);
  server.on("/relay/status", routeRelayStatus);
  server.on("/relay/on", routeRelayOn);
  server.on("/relay/off", routeRelayOff);
  server.on("/set/on/urls", routeSetOnUrls);
  server.on("/set/off/urls", routeSetOffUrls);  
  server.onNotFound(routeNotFound);
  server.begin();
  Serial.println("HTTP server started");    
}


void loop()
{
  server.handleClient();
  
  //delay(200);
  //Serial.println(digitalRead(SONOFF_BUTTON));
  switch (cmd) {
    case CMD_WAIT:
      break;
    case CMD_BUTTON_CHANGE:
      int currentState = digitalRead(SONOFF_BUTTON);
      if (currentState != buttonState) {
        if (buttonState == LOW && currentState == HIGH) {
          long duration = millis() - startPress;
          if (duration < 1000) {
            Serial.println("short press - toggle power");
            toggle();
          } else if (duration < 5000) {
            Serial.println("medium press - reset");
            restart();
          } else if (duration < 60000) {
            Serial.println("long press - reset settings");
            reset();
          }
        } else if (buttonState == HIGH && currentState == LOW) {
          startPress = millis();
        }
        buttonState = currentState;
      }
      break;
  }

  relayTemperatureToggle();
}




