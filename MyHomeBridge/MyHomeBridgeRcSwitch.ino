
// char buffer[128];
void rcSwitchRead() {
  if (rcSwitch.available()) {
    int value = rcSwitch.getReceivedValue();
    if (value) {
      Serial.print("Received ");
      Serial.print( rcSwitch.getReceivedValue() );
      Serial.print(" / ");
      Serial.print( rcSwitch.getReceivedBitlength() );
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println( rcSwitch.getReceivedProtocol() );
      // sprintf(buffer, "{\"code\": %d, \"bit\": %d, \"protocol\": %d, \"pulse\": %d}", rcSwitch.getReceivedValue(), rcSwitch.getReceivedBitlength(), rcSwitch.getReceivedProtocol(), rcSwitch.getReceivedDelay());

//      remoteTopicPub.publish("rcswitch " + String(rcSwitch.getReceivedValue()) + " " + String(rcSwitch.getReceivedBitlength()) + " " + String(rcSwitch.getReceivedProtocol()));
    }
    rcSwitch.resetAvailable();
  }
}
