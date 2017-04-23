
// char buffer[128];
void rcSwitchRead() {
  if (rcSwitch.available()) {
    int value = rcSwitch.getReceivedValue();
    if (value) {
      /*if (rcSwitch.getReceivedValue() == 333251) {
        smartwaresSend("1010100110101001011010100110011010100110011010011001011010101010");
        smartwaresSend("1010100110101001011010100110011010100110011010011001011010101010");
        smartwaresSend("1010100110101001011010100110011010100110011010011001011010101010");
        smartwaresSend("1010100110101001011010100110011010100110011010011001011010101010");
        smartwaresSend("1010100110101001011010100110011010100110011010011001011010101010");
        smartwaresSend("1010100110101001011010100110011010100110011010011001011010101010");
        smartwaresSend("1010100110101001011010100110011010100110011010011001011010101010");
        Serial.println("Do it");
      }*/
      Serial.print("Received ");
      Serial.print( rcSwitch.getReceivedValue() );
      Serial.print(" / ");
      Serial.print( rcSwitch.getReceivedBitlength() );
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println( rcSwitch.getReceivedProtocol() );
      // sprintf(buffer, "{\"code\": %d, \"bit\": %d, \"protocol\": %d, \"pulse\": %d}", rcSwitch.getReceivedValue(), rcSwitch.getReceivedBitlength(), rcSwitch.getReceivedProtocol(), rcSwitch.getReceivedDelay());

      send("rcswitch " + String(rcSwitch.getReceivedValue()) + " " + String(rcSwitch.getReceivedBitlength()) + " " + String(rcSwitch.getReceivedProtocol()));
    }
    rcSwitch.resetAvailable();
  }
}


// on  1010100110101001011010100110011010100110011010011001011010101010
// off 1010100110101001011010100110011010100110011010011001010110101010
