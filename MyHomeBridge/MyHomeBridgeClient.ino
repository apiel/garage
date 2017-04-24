void send(String msg, byte force = 0) 
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
    // client.readStringUntil('\r');
    int incomingByte;
    String line = "";
    while ((incomingByte = client.read()) != -1) {
      line += char(incomingByte);
    }

    int delimiter = line.indexOf(' ');
    if (delimiter != -1) {
      String action = line.substring(0, delimiter);
      String data = line.substring(delimiter+1, line.indexOf('\n')); // line.indexOf('\n') remove possible \n

      Serial.println("Action: " + action + " data: " + data);
      if (action == "homeeasy") {
        smartwaresSendMulti(data);
      } 
    }
    // Serial.print(line);
  }  
}

