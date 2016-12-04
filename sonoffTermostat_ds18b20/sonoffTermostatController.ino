void initController() {
  server.on("/power/status", routePowerStatus);
  server.on("/power/toggle", routePowerToggle);
  server.on("/get/on/temperature", routeGetOnTemperature);
  server.on("/get/off/temperature", routeGetOffTemperature);
  server.on("/temperature/status", routeTemperatureStatus);
  server.on("/relay/status", routeRelayStatus);
  //server.on("/get", routeGet); // we could implement get
  server.on("/set", routeSet);
  server.on("/firmware/update", routeFirmwareUpdate);
  server.on("/", routeRoot);
  server.onNotFound(routeNotFound);  
}

boolean isValidVariable(String varName) {
  return varName == "OnTemperature"
      || varName == "OffTemperature"
      || varName == "onUrls"
      || varName == "offUrls"
      || varName == "mockTemp";
}

void routeSet() {
  int pos = server.args();
  if (pos > 0) {
    while(pos-- > 0) {
      String varName = server.argName(pos);
      if (isValidVariable(varName)) {
        String value = server.arg(pos);
        Serial.println("Set variable " + varName + ": " + value);    
        saveFile(varName, value);    
      }
      else {
        server.send ( 400, "text/plain", "Invalid variable name. " + varName);
        break;
      }
    }
    server.send ( 200, "text/plain", "Variable set.");
  }
  else {
    server.send ( 400, "text/plain", "Set value parameter missing. Please provide a variable to set.");
  }
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

//-------------------------------------------------------------

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

