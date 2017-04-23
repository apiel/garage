unsigned int latchStage = 0;
signed int bitCount = 0;
byte bit = 0; // we could skip bit
byte prevBit = 0; // we could skip prevBit

String bits = "";

void smartwaresRead(unsigned int pulseWidth) { // homeeasy
    if(pulseWidth > 9480 && pulseWidth < 11500)
    { // pause between messages
      // Serial.println("latch");
      latchStage = 1;
      
      bits = "";
      bitCount = 0;      
    }
    else if(latchStage == 1 && pulseWidth > 2350 && pulseWidth < 2750)
    { // advanced protocol latch
      latchStage = 2;
    }
    else if(latchStage == 2)
    { // advanced protocol data
      
      if(pulseWidth > 130 && pulseWidth < 450)
      {
        bit = 0;
        bits += "0";
      }
      else if(pulseWidth > 950 && pulseWidth < 1450)
      {
        bit = 1;
        bits += "1";
      }
      else
      { // start over if the low pulse was out of range        
        // Serial.println("easyhome error out of range " + String(pulseWidth)); 
      }
      
      if(bitCount % 2 == 1)
      {
        if((prevBit ^ bit) == 0)
        { // must be either 01 or 10, cannot be 00 or 11
          latchStage = 0;
          // Serial.println("easyhome error bit " + String(pulseWidth)); 
        }
      }
      
      prevBit = bit;
      bitCount++;
      
      if(bitCount == 64) // instead of bitcound, we could use bits.length()
      { // message is complete
        latchStage = 0;
        Serial.println("easyhome binary: " + bits);
//        remoteTopicPub.publish("easyhome " + String(bits)); // .c_str()
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

