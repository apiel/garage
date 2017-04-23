void remoteCallback(char *data, uint16_t len) {
  Serial.print("Hey we're in a remote callback, the value is: ");
  Serial.println(data);
}

void MQTT_init() {
  remoteTopicSub.setCallback(remoteCallback);
  mqtt.subscribe(&remoteTopicSub);    
}

bool MQTT_connect() {
  int8_t ret;

  if (!mqtt.connected()) {
    Serial.print("Connecting to MQTT... ");
    if ((ret = mqtt.connect()) != 0) {
       Serial.println(mqtt.connectErrorString(ret));
       return false;
    }
    Serial.println("MQTT Connected!");    
  }
  return true;
}

void MQTT_read() {
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(0))) {
    if (subscription == &remoteTopicSub) {
      Serial.print("Got remote: ");
      Serial.println((char *)remoteTopicSub.lastread);
    }
  }  
}

