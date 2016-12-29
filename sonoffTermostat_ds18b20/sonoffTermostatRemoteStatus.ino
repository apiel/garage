long lastCheckForRemoteStatusUrl = millis();
String remoteStatusUrl;

void remoteStatusUrlInit() {
  remoteStatusUrl = readFile("remoteStatusUrl", "");
}

bool remoteStatusUrlCheck() {
  //_logln(remoteStatusUrl);
  if (remoteStatusUrl.length()) {
   if (powerIsOn() && millis() - lastCheckForRemoteStatusUrl > 60000) { // every minute
     lastCheckForRemoteStatusUrl = millis();
     _logln("Call remote status url: ");
     _logln(remoteStatusUrl);
     int httpCode = wifi.callUrl(remoteStatusUrl);
     //_log("[HTTP] GET... code:");
     //_logln(httpCode); //tostring
     if(httpCode > 0 && httpCode == HTTP_CODE_OK) {
       _logln(wifi.lastPayload);
       if (wifi.lastPayload.indexOf("\"on\"") > -1 ) {
         _logln("Remote status url on, turn on relay.");
         turnOn();
       }
       else {
         _logln("Remote status url not on, turn off relay.");
         turnOff();
       }
     } 
     else {
       _logln("Cant reach remote status url, turn off relay.");
       turnOff();
     }
   }
   //_logln("Remote url on");
   return true;
  }
  //_logln("Remote url off");
  return false;
}
