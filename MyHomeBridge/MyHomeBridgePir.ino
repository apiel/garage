bool prevPirValue;
bool pirValue;

void pirRead() {
  pirValue = digitalRead(pinPir);
  if (pirValue != prevPirValue) {
    prevPirValue = pirValue;
    Serial.println("Pir changed: " + String(pirValue));
    send("pir " + String(pirValue));
  }
}

