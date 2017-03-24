String Version = "0.0.2";

void initController() {
  server.on("/relay", routeRelay);
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
  server.send ( 200, "text/plain", "Sonoff Dual Version " + Version);
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
  if (server.hasArg("id")) {
    int id = server.arg("id").toInt();
    if (power == 3 || id+1 == power) {
      server.send ( 200, "text/plain", "{\"status\": \"on\"}");
    }
    else {
      server.send ( 200, "text/plain", "{\"status\": \"off\"}");
    }
  }
  else {
    server.send ( 200, "text/plain", "{\"status\": \"" + String(power) + "\"}");
  }
}

void routeRelay() {
  _logln("Route relay");
  if (server.hasArg("id"), server.hasArg("state")) {
    relayStatus(server.arg("id").toInt(), server.arg("state").toInt());   
  }
  else if (server.hasArg("power")) {
    relayStatus(server.arg("power").toInt());
  }
  else {
    relayStatus(0, 0);
    relayStatus(1, 0);
  }
  server.send ( 200, "text/plain", "ok " + String(power)); 
}
