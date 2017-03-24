String Version = "0.0.1";

void initController() {
  server.on("/relay", routeRelay);
  server.on("/value", routeValue);
  server.on("/gpio", routeGpio);
  server.on("/status", routeStatus);
  server.on("/firmware/update", routeFirmwareUpdate);
  server.on("/restart", routeRestart);
  server.on("/", routeRoot);
  server.onNotFound(routeNotFound);  
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

void routeStatus() {
  _logln("Route status");
  String output = "Thermostat status\n\nVersion: " + Version + "\n\n";

  for(int gpio = 0; gpio < 6; gpio++) {
    pinMode(gpio, INPUT);
    int value = digitalRead(gpio);    
    output += "Gpio: " + String(gpio) + " value: " +  String(value) + "\n";
  }

  for(int gpio = 9; gpio < 11; gpio++) {
    pinMode(gpio, INPUT);
    int value = digitalRead(gpio);    
    output += "Gpio: " + String(gpio) + " value: " +  String(value) + "\n";
  }  

  for(int gpio = 12; gpio < 17; gpio++) {
    pinMode(gpio, INPUT);
    int value = digitalRead(gpio);    
    output += "Gpio: " + String(gpio) + " value: " +  String(value) + "\n";
  }    
  
  server.send ( 200, "text/plain", output);
}

void routeValue() {
  _logln("Route power status");
  int gpio = 0;
  if (server.hasArg("gpio")) {
    gpio = server.arg("gpio").toInt();
  }
  pinMode(gpio, INPUT);
  int value = digitalRead(gpio);
  server.send ( 200, "text/plain", String(value)); 
}

void routeGpio() {
  _logln("Route power status");
  int gpio = 0;
  int value = LOW;
  if (server.hasArg("gpio")) {
    gpio = server.arg("gpio").toInt();
  }
  if (server.hasArg("value")) {
    value = server.arg("value").toInt();
  }  
  pinMode(gpio, OUTPUT);
  digitalWrite(gpio, value);
  server.send ( 200, "text/plain", String(value)); 
}

void routeRelay() {
  _logln("Route relay");
  if (server.hasArg("id"), server.hasArg("state")) {
    relayStatusBis(server.arg("id").toInt(), server.arg("state").toInt());   
  }
  else {
    relayStatusBis(0, 0);
    relayStatusBis(1, 0);
  }
  server.send ( 200, "text/plain", "ok " + String(power)); 
}
