void tick()
{
  int state = digitalRead(SONOFF_LED);  // get the current state of GPIO1 pin
  digitalWrite(SONOFF_LED, !state);     // set pin to the opposite state
}

void callUrls(String urls) {
  int currentPos = 0;
  int pos;
  Serial.println("Call urls: ");
  while ((pos = urls.indexOf('|', currentPos)) > -1) {
    String url = urls.substring(currentPos, pos);
    wifi.callUrl(url);
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

  //Serial.print("Read file: ");
  //Serial.println(file);

  File configFile = SPIFFS.open(file, "r");
  if (!configFile) {
    Serial.println("Failed to open file for reading");
  }
  else {
      value = configFile.readString();       
  }
  return value;
}

void restart() {
  ESP.reset();
  delay(1000);
}

