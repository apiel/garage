unsigned int pirValue;

void pirChanged() {
  pirValue = digitalRead(pinPir);
  Serial.println("Pir changed: " + String(pirValue));
}

