WiFiClient logClient;
WiFiServer logServer(23);

void initLogServer() {
  logServer.begin();
}

void handleLogServer() {
  if (logServer.hasClient()){
    logClient = logServer.available(); 
  }  
}

void _logln(String val) {
  Serial.println(val);
  if (logClient && logClient.connected()) {
    logClient.println(val);  
  }
}

void _log(String val) {
  Serial.print(val);
  if (logClient && logClient.connected()) {
    logClient.print(val);  
  }
}

