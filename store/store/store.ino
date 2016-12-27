#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#include <MyWifiSettings.h>
const char* wifiSsid = MYWIFISSID;
const char* wifiPassword = MYWIFIPASSWORD;

const int OFF = 1;
const int ON = 0;

IPAddress ip(192,168,0,30);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

int pinSensor = 16;
int motorStatus = -1;
unsigned long motorTimer = 0;

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


long lastCheck = millis();
bool checkWifiByUrlCall() {
  bool isValid = true;
  if (millis() - lastCheck > 60000) { // check every minute
    lastCheck = millis();
    Serial.print("Call check url: ");
    int httpCode = callUrl("http://192.168.0.1/");
    Serial.println(httpCode);
    isValid = httpCode == 200;
  }

  return isValid;
}

void checkWifi() {       
  if (!isConnectedToWifi() || !checkWifiByUrlCall()) {
      Serial.println("\nDisconnected from Wifi, reset in 5 sec.");
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


void changeMotorStatus(int _motorStatus) {
  if (motorStatus != _motorStatus) {
    motorStatus = _motorStatus;
    Serial.print("Change motor status: ");
    Serial.println(motorStatus);
    digitalWrite(0, motorStatus);
    if (motorStatus == ON) {
      motorTimer = millis();
    }
    else {
      motorTimer = 0;
    }
  }
}

bool isOpen() {
  pinMode(pinSensor, OUTPUT);  
  digitalWrite(pinSensor, 1);
  pinMode(pinSensor, INPUT);    
  return digitalRead(pinSensor) == 0; // pin connect to GND when opened
}

void startMotor(int val) { // val = 1 open val = 0 close
    changeMotorStatus(OFF);
    digitalWrite(5, val);
    digitalWrite(4, val); 
    changeMotorStatus(ON);  
}

void doOpen() {
    Serial.println("Open store");
    server.send ( 200, "text/plain", "{\"status\": \"opening\"}");
    startMotor(1);
}

void doClose() {
    Serial.println("Close store");
    server.send ( 200, "text/plain", "{\"status\": \"closing\"}");
    startMotor(0);
    //delay(3000);
}

void doStop() {
  Serial.println("Stop motor");
  server.send ( 200, "text/plain", "{\"status\": \"stopped\"}");
  changeMotorStatus(OFF);  
}

void doStart() {
  Serial.println("Start motor");
  server.send ( 200, "text/plain", "{\"status\": \"started\"}");  
  changeMotorStatus(ON);
}

void routeToggle() {
  Serial.println("Route toggle");
  if (motorStatus == ON) {
    doStop();
  }
  else if (isOpen()) {
    doClose();
  }
  else {
    doOpen();
  }
}

void routeStatus() {
  Serial.println("Route status");
  if (isOpen()) {
    server.send ( 200, "text/plain", "{\"status\": \"opened\"}");
  }
  else {
    server.send ( 200, "text/plain", "{\"status\": \"closed\"}");
  }
}

void routeGpio() {
  Serial.println("Gpio write");
  if (!server.hasArg("pin") || !server.hasArg("value")) {
    server.send(400, "text/plain", "Gpio read parameter missing. Please provide pin and value");    
  }
  else {
    int pin = server.arg("pin").toInt();
    int value = server.arg("value").toInt();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, value);
    server.send(200, "text/plain", "DONE");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(5, OUTPUT); //=1
  pinMode(2, OUTPUT); //=4
  pinMode(0, OUTPUT); //=3
  pinMode(4, OUTPUT); //=2
  //pinMode(pinSensor, INPUT);  
  digitalWrite(5, 1);
  digitalWrite(2, 1);
  digitalWrite(4, 1);
  changeMotorStatus(OFF);
  connectToWifi();
  server.on("/", routeRoot);
  server.on("/status", routeStatus);
  server.on("/toggle", routeToggle);
  server.on("/open", doOpen);
  server.on("/close", doClose);
  server.on("/stop", doStop);
  server.on("/start", doStart);
  server.on("/gpio", routeGpio);
  server.onNotFound(routeNotFound);
  server.begin();
  Serial.println("HTTP server started");  
}

void loop() {
  server.handleClient();
  checkWifi();

  if (
    motorStatus == ON //not motorTimer > 0
    &&
    (
      (isOpen() && motorTimer < millis()-3000) 
      || 
      motorTimer+30000 < millis()
    )){
      Serial.print(millis());
      Serial.print(" Stop motor trigger >> motor start: ");
      Serial.println(motorTimer+0);
      changeMotorStatus(OFF);
  }
}
