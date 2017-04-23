
#include <MyHomeBridgeWifi.h>
#include <MyWifiSettings.h>

#include <math.h>
#include <RCSwitch.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

MyHomeBridgeWifi wifi;

int pinRfReceiver = 5; // wemos d1
int pinRfEmitter = 15; // wemos d8
int pinPir = 0; // wemos d3
int pinDHT22 = 12; // wemos d6

int lastRead;

RCSwitch rcSwitch = RCSwitch();

#define AIO_SERVER      "192.168.0.13"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "helo123"
#define AIO_KEY         "key"

WiFiClient client;
bool mqttIsConnected = false;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish pirTopicPub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/pir");
Adafruit_MQTT_Publish photocellTopicPub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/photocell");
Adafruit_MQTT_Publish humidityTopicPub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/humidity");
Adafruit_MQTT_Publish temperatureTopicPub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/temperature");
Adafruit_MQTT_Publish remoteTopicPub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/remote");
Adafruit_MQTT_Subscribe remoteTopicSub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/remote");

const char *APssid = "esp";
const char *APpassword = "abc123";

// #define AP_SSID       "esp"
// #define AP_PASSWORD   "abc123"

void setup() {
  Serial.begin(115200);

  wifi.accessPointConfig(APssid, APpassword);
  // wifi.accessPointConfig(AP_SSID, AP_PASSWORD);
  wifi.connect(MYWIFISSID, MYWIFIPASSWORD);
  // wifi.connect();  
  
  pinMode(pinRfEmitter, OUTPUT);  
  pinMode(pinRfReceiver, INPUT);
  pinMode(pinPir, INPUT);
  rcSwitch.enableReceive(pinRfReceiver);
  attachInterrupt(digitalPinToInterrupt(pinPir), pirChanged, CHANGE);

  MQTT_init();

/*
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
//  */
/*
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101010");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101010");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101010");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101010");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101010");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101010");
//  */  
  /*
  // Received 333571 / 24bit Protocol: 1
  rcSwitch.enableTransmit(pinRfEmitter);
  rcSwitch.setProtocol(1);
  rcSwitch.setRepeatTransmit(25);
  rcSwitch.send(333571, 24);
//  */
}

void loop() {
  wifi.check();

  if (wifi.isConnected) {
      mqttIsConnected = MQTT_connect();
      MQTT_read();
    // if (mqttIsConnected = MQTT_connect()) {
      unsigned int pulseWidth = pulseIn(pinRfReceiver, LOW);
      rcSwitchRead();
      smartwaresRead(pulseWidth);
      readLight();
      if (millis()-lastRead > 30000) { // read temp only every 30 seconds
        lastRead = millis();
        Serial.println("Read");
        readTemp();
      }    
    // }
  }
}
