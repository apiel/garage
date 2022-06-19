#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define PIN_BTN 0
#define PIN_LED 13
#define PIN_RELAY 12
#define RELAY_ON 1
#define RELAY_OFF 0

unsigned long startTime;
int relayState = RELAY_OFF;
int mode = 0;

void relayOn()
{
  Serial.println("Set relay ON.");
  relayState = RELAY_ON;
  digitalWrite(PIN_RELAY, relayState);
}

void relayOff()
{
  Serial.println("Set relay OFF.");
  relayState = RELAY_OFF;
  digitalWrite(PIN_RELAY, relayState);
}

int initBtnState = 0;
int lastBtnState = 0;
void handleBtn()
{
  int val = digitalRead(PIN_BTN);
  if (lastBtnState != val)
  {
    lastBtnState = val;
    if (val != initBtnState)
    {
      Serial.printf("Change mode %d\n", mode + 1);
      mode = ((mode + 1) % 3);
      EEPROM.write(0, mode);
      EEPROM.commit();
      Serial.println(mode);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BTN, INPUT);

  initBtnState = lastBtnState = digitalRead(PIN_BTN);

  EEPROM.begin(1);
  mode = EEPROM.read(0);
  Serial.printf("Mode %d\n", mode + 1);

  startTime = millis();
  relayOn();
}

void loop()
{
  if (relayState == RELAY_ON)
  {
    if (mode == 0 && millis() > 30 * 60 * 1000)
    {
      Serial.printf("Off after 30min (%d)\n", millis());
      relayOff();
    }
    if (mode == 1 && millis() > 45 * 60 * 1000)
    {
      Serial.printf("Off after 45min (%d)\n", millis());
      relayOff();
    }
    if (mode == 2 && millis() > 60 * 60 * 1000)
    {
      Serial.printf("Off after 1h (%d)\n", millis());
      relayOff();
    }

    for (int i = 0; i < mode + 1; i++)
    {
      digitalWrite(PIN_LED, LOW);
      delay(100);
      digitalWrite(PIN_LED, HIGH);
      delay(100);
    }
  }

  Serial.println("Wait...");
  delay(250);
  handleBtn();
  delay(250);
  handleBtn();
  delay(250);
  handleBtn();
  delay(250);
  handleBtn();
}
