String Version = "1.0.4";

void initController() {
  server.on("/power/status", routePowerStatus);
  server.on("/status", routeStatus);
  server.on("/power/toggle", routePowerToggle);
  server.on("/power/on", routePowerOn);
  server.on("/power/off", routePowerOff);
  server.on("/power/plan", routePowerPlan);
  server.on("/get/on/temperature", routeGetOnTemperature);
  server.on("/get/off/temperature", routeGetOffTemperature);
  server.on("/temperature/status", routeTemperatureStatus);
  server.on("/relay/status/force", routeRelayStatusForce);
  server.on("/relay/status", routeRelayStatus);
  server.on("/get", routeGet);
  server.on("/set", routeSet);
  server.on("/firmware/update", routeFirmwareUpdate);
  server.on("/restart", routeRestart);
  server.on("/", routeRoot);
  server.onNotFound(routeNotFound);  
}

// We should load all the variable in a list at the begening
// this could be part of a class variable
boolean isValidVariable(String varName) {
  return varName == "OnTemperature"
      || varName == "OffTemperature"
      || varName == "onUrls"
      || varName == "offUrls"
      || varName == "ipLastNumber"
      || varName == "mockTemp"
      || varName == "remoteStatusUrl";
}

// this could be part of a class variable
void routeSet() {
  int pos = server.args();
  if (pos > 0) {
    while(pos-- > 0) {
      String varName = server.argName(pos);
      if (isValidVariable(varName)) {
        String value = server.arg(pos);
        _logln("Set variable " + varName + ": " + value);    
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

void routeGet() {
  _logln("routeGet");
  if (server.hasArg("var")) {
    String varName = server.arg("var");
    String value = readFile(varName, "");

    server.send ( 200, "text/plain", value);
  }
  else {
    server.send ( 400, "text/plain", "Get value parameter missing. Please provide a var.");
  }  
}

void routeNotFound() {
  _logln("Route Not Found ");
  _logln(server.uri());
  server.send ( 404, "text/plain", "Not Found!");
}

void routeRoot() {
  _logln("Route Root");
  server.send ( 200, "text/plain", "Hello. Version " + Version);
}

void routeRestart() {
  _logln("Route Restart");
  restart();
}

void routeFirmwareUpdate() {
  _logln("routeFirmwareUpdate");
  if (server.hasArg("ip")) { 
    String ip = server.arg("ip");
    _logln(ip);

    server.send ( 200, "text/plain", "Try to update firmware.");
    ESPhttpUpdate.update(ip, 80, "/firmware.bin");
  }
  else {
    server.send ( 400, "text/plain", "Update firmware parameter missing. Please provide ip/domain.");
  }  
}

//-------------------------------------------------------------

void routeGetOnTemperature() {
  _logln("Route GetOnTemperature");
  String response =  "{\"status\": " + String(getOnTemperature()) + "}";
  server.send ( 200, "text/plain", response);
}

void routeGetOffTemperature() {
  _logln("Route GetOffTemperature");
  String response =  "{\"status\": " + String(getOffTemperature()) + "}";
  server.send ( 200, "text/plain", response);
}

void routeRelayStatus() {
  _logln("Route relay status");
  if (powerIsOn() && relayIsOn()) {
    server.send ( 200, "text/plain", "{\"status\": \"on\"}");
  }
  else {
    server.send ( 200, "text/plain", "{\"status\": \"off\"}");
  }
}

void routeRelayStatusForce() {
  _logln("Route relay status force");
  if (relayIsOnForce()) {
    server.send ( 200, "text/plain", "{\"status\": \"on\"}");
  }
  else {
    server.send ( 200, "text/plain", "{\"status\": \"off\"}");
  }
}

void routePowerToggle() {
  powerToggle();
  routePowerStatus();
}

void routePowerOn() {
  powerOn();
  routePowerStatus();
}

void routePowerOff() {
  powerOff();
  routePowerStatus();
}

void routePowerStatus() {
  _logln("Route power status");
  if (powerIsOn()) {
    server.send ( 200, "text/plain", "{\"status\": \"on\"}");  
  }
  else {
    server.send ( 200, "text/plain", "{\"status\": \"off\"}");  
  }
}

void routeTemperatureStatus() {
  _logln("Route temperature status");
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

void routeStatus() {
  _logln("Route status");
  String output = "Thermostat status\n\nVersion: " + Version + "\n";
  
  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);
  output += "Temperature: " + String(t) + "\n";

  output += "Power: " + String(powerIsOn()?"on":"off") + "\n";
  output += "powerState: " + String(powerState) + "\n";

  output += "Relay: " + String(powerIsOn() && relayIsOn()?"on":"off") + "\n";
  output += "RelayIsOnForce: " + String(relayIsOnForce()?"on":"off") + "\n";
  output += "relayState: " + String(relayState) + "\n";  

  output += "On temperature: " + String(getOnTemperature()) + "\n";
  output += "Off temperature: " + String(getOffTemperature()) + "\n";

  server.send ( 200, "text/plain", output);
}

void routePowerPlan() {
  _logln("Route power status");
  int when = 0;
  int forHowLong = 60; // Default 60min
  if (server.hasArg("when")) {
    when = server.arg("when").toInt();
  }
  if (server.hasArg("for")) {
    forHowLong = server.arg("for").toInt();
  }  
  powerOnIn = when*60000 + millis();
  powerOffIn = powerOnIn + forHowLong*60000;
}

