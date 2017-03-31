unsigned int latchStage = 0;
signed int bitCount = 0;
byte bit = 0;
byte prevBit = 0;

unsigned long sender = 0;
unsigned int recipient = 0;
byte command = 0;
bool group = false;

void smartwaresRead(unsigned int pulseWidth) { // homeeasy
    if(pulseWidth > 9480 && pulseWidth < 11500)
    { // pause between messages
      Serial.println("latch");
      latchStage = 1;
    }
    else if(latchStage == 1 && pulseWidth > 2350 && pulseWidth < 2750)
    { // advanced protocol latch
      latchStage = 2;
      sender = 0;
    }
    else if(latchStage == 2)
    { // advanced protocol data
      
      if(pulseWidth > 130 && pulseWidth < 450)
      {
        bit = 0;
      }
      else if(pulseWidth > 950 && pulseWidth < 1450)
      {
        bit = 1;
      }
      else
      { // start over if the low pulse was out of range        
        Serial.println("easyhome error out of range " + String(pulseWidth)); 
//        latchStage = 0;
//        bitCount = 0;
//        recipient = 0;
      }
      
      if(bitCount % 2 == 1)
      {
        if((prevBit ^ bit) == 0)
        { // must be either 01 or 10, cannot be 00 or 11
          latchStage = 0;
          bitCount = -1;
          Serial.println("easyhome error bit " + String(pulseWidth)); 
        }
        else if(bitCount < 53)
        { // first 26 data bits
          
          sender <<= 1;
          sender |= prevBit;
        }
        else if(bitCount == 53)
        { // 26th data bit
          group = prevBit;
        }
        else if(bitCount == 55)
        { // 27th data bit
          command = prevBit;
        }
        else
        { // last 4 data bits
          recipient <<= 1;
          recipient |= prevBit;
        }
      }
      
      prevBit = bit;
      bitCount++;
      
      if(bitCount == 64)
      { // message is complete
        Serial.print("easyhome success: " + String(sender));     
        Serial.print(" r: " + String(recipient));
        Serial.print(" c: " +String(command));
        Serial.println(" g: " +String(group));
        
        sender = 0;
        recipient = 0;
        latchStage = 0;
        bitCount = 0;
      }
    }
}

