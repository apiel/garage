byte latchStage = 0;
String bits = "";

void smartwaresRead(unsigned int pulseWidth) { // homeeasy
    if(pulseWidth > 9480 && pulseWidth < 11500) {
      latchStage = 1; 
      bits = "";      
    }
    else if(latchStage == 1 && pulseWidth > 2350 && pulseWidth < 2750) {
      latchStage = 2;
    }
    else if(latchStage == 2) {
      if(pulseWidth > 130 && pulseWidth < 450) {
        bits += "0";
      }
      else if(pulseWidth > 950 && pulseWidth < 1450) {
        bits += "1";
      }

      if(bits.length() % 2 == 0) {
        int lastBit = bits.length() - 1;
        if((bits[lastBit-1] ^ bits[lastBit]) == 0)
        { // must be either 01 or 10, cannot be 00 or 11
          latchStage = 0;
        }
      }
            
      if(bits.length() == 64) { // instead of bitcound, we could use bits.length()
        latchStage = 0;
        Serial.println("easyhome binary: " + bits);
        send("easyhome " + String(bits));
      }
    }
}

void doPulse(int duration)
{
    digitalWrite(pinRfEmitter, HIGH);
    delayMicroseconds(300);
    digitalWrite(pinRfEmitter, LOW);
    delayMicroseconds(duration);
}

void smartwaresSend(String code) {
    doPulse(10500);
    doPulse(2500);
    Serial.println(":smartwaresSend: " + code);
    for(int pos = 0; pos < code.length(); pos++) {
      if (code[pos] == '0') {
        doPulse(300);
      }
      else {
        doPulse(1200);
      }
    }
}

