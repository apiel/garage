void send(String msg) 
{
    // we could give the choice between socket message or http request
    client.print(device + " " + msg + "\n");  
}

bool connect() 
{
  if (!client.connected()) {
    Serial.println("Connect to bridge");
    if (!client.connect(host, port)) {
      Serial.println("connection failed");
      return false;
    }
    Serial.println("Connect to bridge");
    send("helo");
  }
  return true;
}

void receive()
{
  if (connect() && client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }  
}

