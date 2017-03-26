#include <RCSwitch.h>

int pinReceiver = 5;

RCSwitch rcSwitch = RCSwitch();

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
    }
    rcSwitch.resetAvailable();
  } 
//  delay(100);
}

unsigned int pulseWidth = 0;
unsigned int latchStage = 0;
bool bbsb2011 = false;
signed int bitCount = 0;
byte bit = 0;
byte prevBit = 0;

unsigned long sender = 0;
unsigned int recipient = 0;
byte command = 0;
bool group = false;

void homeeasyRead() {
    if(latchStage == 0 && pulseWidth > 9480 && pulseWidth < 11500)
    { // pause between messages
      Serial.println("latch");
      latchStage = 1;
      //latchStage = 3; // to remove
      //sender = 0; // to remove
    }
    else if(latchStage == 1 && pulseWidth > 2350 && pulseWidth < 2750)
    { // advanced protocol latch
      
      latchStage = 3;
      sender = 0;
    }
    else if(latchStage == 3)
    { // advanced protocol data
      
      if(pulseWidth > 150 && pulseWidth < 365)
      {
        bit = 0;
      }
      else if(pulseWidth > 1000 && pulseWidth < 1360)
      {
        bit = 1;
      }
      else
      { // start over if the low pulse was out of range
/*        Serial.print("easyhome error: ");   
        Serial.println(String(pulseWidth));
        Serial.println(String(bitCount));
        Serial.println(String(sender));
        Serial.println(String(recipient));
        Serial.println(String(command));
        Serial.println(String(group));*/
        
        latchStage = 0;
        bitCount = 0;
        
        recipient = 0;
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

void setup() {
  Serial.begin(115200);
  pinMode(pinReceiver, INPUT);  
  rcSwitch.enableReceive(pinReceiver);

  delay(1000);
  Serial.println("Hello");  
}

void loop() {
  rcSwitchRead();
//  Serial.println(digitalRead(pinReceiver)); 
//  Serial.println(pulseIn(pinReceiver, LOW));
  pulseWidth = pulseIn(pinReceiver, LOW);
//  Serial.println(pulseWidth);
  homeeasyRead();
}
