long lastCheckForRemoteStatusUrl = millis();
String remoteStatusUrl;

void remoteStatusUrlInit() {
  String remoteStatusUrl = readFile("remoteStatusUrl", "");
}

bool remoteStatusUrlCheck() {
  if (remoteStatusUrl.length()) {
   if (powerIsOn() && millis() - lastCheckForRemoteStatusUrl > 60000) { // every minute
     lastCheckForRemoteStatusUrl = millis();
     Serial.print("Call remote status url: ");
     Serial.println(remoteStatusUrl);
     int httpCode = wifi.callUrl(remoteStatusUrl);
     Serial.printf("[HTTP] GET... code: %d\n", httpCode);
     if(httpCode > 0 && httpCode == HTTP_CODE_OK) {
       Serial.println(wifi.lastPayload);
       if (wifi.lastPayload.indexOf("\"on\"") > -1 ) {
         Serial.println("Remote status url on, turn on relay.");
         turnOn();
       }
       else {
         Serial.println("Remote status url not on, turn off relay.");
         turnOff();
       }
     } 
     else {
       Serial.println("Cant reach remote status url, turn off relay.");
       turnOff();
     }
   }
   return true;
  }
  return false;
}
