int prevLight;

void readLight() {
  int light = analogRead(0);   

  if (abs(light - prevLight) > 25) {
    prevLight = light;
    Serial.println("Light changed: " + String(light));

    // photocellTopicPub.publish(light);
  }   
}

