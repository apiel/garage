#include <math.h>
#include <RCSwitch.h>

int pinRfReceiver = 5;
int pinRfEmitter = 15;
int pinPir = 4;
int pinDHT22 = 0;

int lastRead;

RCSwitch rcSwitch = RCSwitch();

void setup() {
  Serial.begin(115200);
  pinMode(pinRfEmitter, OUTPUT);  
  pinMode(pinRfReceiver, INPUT);
  pinMode(pinPir, INPUT);
  rcSwitch.enableReceive(pinRfReceiver);
  attachInterrupt(digitalPinToInterrupt(pinPir), pirChanged, CHANGE);

  delay(1000);
  Serial.println("Hello");

  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
  smartwaresSend("1010100110101001011010100110011010100110011010011001011010101001");
}

void loop() {
  unsigned int pulseWidth = pulseIn(pinRfReceiver, LOW);
  rcSwitchRead();
  smartwaresRead(pulseWidth);
  readLight();
  if (millis()-lastRead > 30000) { // read temp only every 30 seconds
    lastRead = millis();
    Serial.println("Read");
    readTemp();
  }
}
