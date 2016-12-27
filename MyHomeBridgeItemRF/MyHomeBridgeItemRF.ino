#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <RCSwitch.h>

#include <MyWifiSettings.h>
const char* wifiSsid = MYWIFISSID;
const char* wifiPassword = MYWIFIPASSWORD;

IPAddress ip(192,168,0,31);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

int pinTransmiter = 16;
int pinReceiver = 5;

RCSwitch mySwitch = RCSwitch();

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
  Serial.print("Connect to WiFi: ");  
  Serial.println(wifiSsid);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(wifiSsid, wifiPassword);
  bool connected = isConnectedToWifi();             
  if (connected) {
      Serial.println("\nConnected to WiFi.");        
      Serial.println(WiFi.localIP());
  }
}

void checkWifi() {
  bool connected = isConnectedToWifi();             
  if (!connected) {
      Serial.println("\nDisconnected from Wifi try to reconnect in 5 sec.");
      delay(5000);
      connectToWifi();
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

void routeSend() {
  Serial.println("Rcswitch send");
  if (server.hasArg("code")) {
    mySwitch.enableTransmit(pinTransmiter);
    mySwitch.setProtocol(server.hasArg("protocol") ? server.arg("protocol").toInt() : 1);
    mySwitch.setPulseLength(server.hasArg("pulse") ? server.arg("pulse").toInt() : 180);
    mySwitch.setRepeatTransmit(server.hasArg("repeat") ? server.arg("repeat").toInt() : 15);
    if (server.hasArg("bit")) {
      mySwitch.send(server.arg("code").toInt(), server.arg("bit").toInt());
    }
    else {
      char code[128];
      server.arg("code").toCharArray(code, 128);
      mySwitch.send(code);
    }
    server.send(200, "text/plain", "Code sent.");
  }
  else {
    server.send(400, "text/plain", "Rcswitch send parameter missing. Please provide code");
  }
}

void routeRead() {
  Serial.println("Rcswitch read");
  
  mySwitch.enableReceive(pinReceiver);

  char buffer[128];
  int retry = 100;
  while(retry--) {
    if (mySwitch.available()) {  
      int value = mySwitch.getReceivedValue();
      
      if (value == 0) {
        Serial.print("Unknown encoding");
      } else {
        //  used for checking received data      
            Serial.print("Received ");       
            Serial.print( mySwitch.getReceivedValue() );      
            Serial.print(" / ");     
            Serial.print( mySwitch.getReceivedBitlength() );      
            Serial.print("bit ");      
            Serial.print("Protocol: ");     
            Serial.println( mySwitch.getReceivedProtocol() );    
            sprintf(buffer, "{\"code\": %d, \"bit\": %d, \"protocol\": %d, \"pulse\": %d}", mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedProtocol(), mySwitch.getReceivedDelay());
            server.send(200, "text/plain", buffer);
      }
      mySwitch.resetAvailable();
    } 
    else {
      delay(100);
    }
  }
  
  if (retry < 1) {
    server.send(200, "text/plain", "Error read.");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(pinTransmiter, OUTPUT);
  pinMode(pinReceiver, INPUT);  

  connectToWifi();
  server.on("/", routeRoot);
  server.on("/send", routeSend);
  server.on("/read", routeRead);

  server.onNotFound(routeNotFound);
  server.begin();
  Serial.println("HTTP server started");  
}

void loop() {
  server.handleClient();
  checkWifi();
}
