#include <math.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include <MyWifiSettings.h>
const char* wifiSsid = MYWIFISSID;
const char* wifiPassword = MYWIFIPASSWORD;

IPAddress ip(192,168,0,88);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

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

double Thermistor(int RawADC) {
 double Temp;
 Temp = log(10000.0*((1024.0/RawADC-1))); 
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15;           
 return Temp;
}

double getTemp() {
  int val;   
  double temp;            
  val=analogRead(0);      
  temp=Thermistor(val);

  return temp;
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
  double t = getTemp();
  if (isnan(t)) {
     server.send ( 200, "text/plain", "{\"status\": \"unknown\"}");
  }
  else {
    String response =  "{\"status\": " + String(t) + "}";
    server.send ( 200, "text/plain", response);
  }
}

void routeFirmwareUpdate() {
  Serial.println("routeFirmwareUpdate");
  if (server.hasArg("url")) {
    String ip = server.arg("url");
    Serial.println(ip);

    server.send ( 200, "text/plain", "Try to update firmware.");
    ESPhttpUpdate.update(ip, 80, "/firmware.bin");
  }
  else {
    server.send ( 400, "text/plain", "Update firmware parameter missing. Please provide ip/domain.");
  }  
}

void routeNotFound() {
  Serial.print("Route Not Found ");
  Serial.println(server.uri());
  server.send ( 404, "text/plain", "Not Found!");
}

void routeRoot() {
  Serial.println("Route Root");
  server.send ( 200, "text/plain", "Hello.\n\n/temperature/status\n/firmware/update"); // todo: create a function listing all route base on server setting
}

void setup()
{
  Serial.begin(115200);

  connectToWifi();
  server.on("/", routeRoot);
  server.on("/temperature/status", routeTemperatureStatus);
  server.on("/firmware/update", routeFirmwareUpdate);
  server.onNotFound(routeNotFound);
  server.begin();
  Serial.println("HTTP server started");    
}


void loop()
{
  server.handleClient();
}

