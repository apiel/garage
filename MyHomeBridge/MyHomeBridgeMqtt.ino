#define AIO_SERVER      "192.168.0.13"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "helo123"
#define AIO_KEY         "key"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

void MQTT_connect() {
  int8_t ret;

  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  else if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");
  if ((ret = mqtt.connect()) != 0) {
     Serial.println(mqtt.connectErrorString(ret));
     return;
  }
  Serial.println("MQTT Connected!");
}
