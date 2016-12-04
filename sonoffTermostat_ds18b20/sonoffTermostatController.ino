void initController() {
  server.on("/", routeRoot);
  server.on("/firmware/update", routeFirmwareUpdate);
  server.on("/power/status", routePowerStatus);
  server.on("/power/toggle", routePowerToggle);
  server.on("/get/on/temperature", routeGetOnTemperature);
  server.on("/get/off/temperature", routeGetOffTemperature);
  server.on("/set/temperature", routeSetTemperature);
  server.on("/temperature/status", routeTemperatureStatus);
  server.on("/relay/status", routeRelayStatus);
  server.on("/relay/on", routeRelayOn);
  server.on("/relay/off", routeRelayOff);
  server.on("/set/on/urls", routeSetOnUrls);
  server.on("/set/off/urls", routeSetOffUrls);  
  //server.on("/set/temperature/url", routeSetTemperatureUrl);
  server.onNotFound(routeNotFound);  
}

void routeGetOnTemperature() {
  Serial.println("Route GetOnTemperature");
  String response =  "{\"status\": " + String(getOnTemperature()) + "}";
  server.send ( 200, "text/plain", response);
}

void routeGetOffTemperature() {
  Serial.println("Route GetOffTemperature");
  String response =  "{\"status\": " + String(getOffTemperature()) + "}";
  server.send ( 200, "text/plain", response);
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

void routeRelayStatus() {
  Serial.println("Route relay status");
  if (relayIsOn()) {
    server.send ( 200, "text/plain", "{\"status\": \"on\"}");
  }
  else {
    server.send ( 200, "text/plain", "{\"status\": \"off\"}");
  }
}

void routePowerToggle() {
  toggle();
  routePowerStatus();
}

void routePowerStatus() {
  Serial.println("Route power status");
  if (powerIsOn()) {
    server.send ( 200, "text/plain", "{\"status\": \"on\"}");  
  }
  else {
    server.send ( 200, "text/plain", "{\"status\": \"off\"}");  
  }
}

void routeRelayOn() {
  turnOn();
  routeRelayStatus();
}

void routeRelayOff() {
  turnOff();
  routeRelayStatus();
}

void routeTemperatureStatus() {
  Serial.println("Route temperature status");
  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);
  if (isnan(t)) {
     server.send ( 200, "text/plain", "{\"status\": \"unknown\"}");
  }
  else {
    String response =  "{\"status\": " + String(t) + "}";
    server.send ( 200, "text/plain", response);
  }
}

void routeFirmwareUpdate() {
  Serial.println("routeFirmwareUpdate");
  if (server.hasArg("url")) {
    String ip = server.arg("url");
    Serial.println(ip);

    server.send ( 200, "text/plain", "Try to update firmware.");
    ESPhttpUpdate.update(ip, 80, "/firmware.bin");
  }
  else {
    server.send ( 400, "text/plain", "Update firmware parameter missing. Please provide ip/domain.");
  }  
}

// All file saving could be combine in one function and could handle multiple save at once
// String argName(int i);          // get request argument name by number
// int args();                     // get arguments count
//  String arg(String name);        // get request argument value by name
//  String arg(int i);              // get request argument value by number
// Foreach args, check argName is a valid file to save data, then save

void routeSetTemperature() {
  Serial.println("routeSetTemperature");
  if (server.hasArg("on") || server.hasArg("off")) {
    if (server.hasArg("on")) {
      String value = server.arg("on");
      offTemperature = value.toFloat();
      saveFile("OnTemperature", value);
    }
    if (server.hasArg("off")) {
      String value = server.arg("off");
      offTemperature = value.toFloat();
      saveFile("OffTemperature", value);
    }    
    server.send ( 200, "text/plain", "Temperature set.");
  }
  else {
    server.send ( 400, "text/plain", "Set OffTemperature parameter missing. Please provide on or/and off.");
  }
}

/*
void routeSetTemperatureUrl() {
  Serial.println("routeSetTemperatureUrl");
  if (server.hasArg("url")) {
    temperatureUrl = server.arg("url");
    saveFile("temperatureUrl", temperatureUrl);    
    Serial.println(temperatureUrl);
    server.send ( 200, "text/plain", "Temperature url set.");
  }
  else {
    server.send ( 400, "text/plain", "Set temperature url parameter missing. Please provide url.");
  }
}
*/

void routeSetOnUrls() {
  Serial.println("routeSetOnUrls");
  if (server.hasArg("urls")) {
    String value = server.arg("urls");
    saveFile("onUrls", value);    
    Serial.println(value);
    server.send ( 200, "text/plain", "urls on set.");
  }
  else {
    server.send ( 400, "text/plain", "Set on urls parameter missing. Please provide urls.");
  }
}

void routeSetOffUrls() {
  Serial.println("routeSetOffUrls");
  if (server.hasArg("urls")) {
    String value = server.arg("urls");
    saveFile("offUrls", value);    
    Serial.println(value);
    server.send ( 200, "text/plain", "urls off set.");
  }
  else {
    server.send ( 400, "text/plain", "Set off urls parameter missing. Please provide urls.");
  }
}

