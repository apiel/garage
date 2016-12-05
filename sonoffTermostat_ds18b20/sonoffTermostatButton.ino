//inverted button state
int buttonState = HIGH;

static long startPress = 0;

const int CMD_WAIT = 0;
const int CMD_BUTTON_CHANGE = 1;

int cmd = CMD_WAIT;

void sonoffButtonTrigger() {
  switch (cmd) {
    case CMD_WAIT:
      break;
    case CMD_BUTTON_CHANGE:
      int currentState = digitalRead(SONOFF_BUTTON);
      if (currentState != buttonState) {
        if (buttonState == LOW && currentState == HIGH) {
          long duration = millis() - startPress;
          if (duration < 1000) {
            _logln("short press - toggle power");
            toggle();
          } else if (duration < 5000) {
            _logln("medium press - reset");
            restart();
          } 
        } else if (buttonState == HIGH && currentState == LOW) {
          startPress = millis();
        }
        buttonState = currentState;
      }
      break;
  }  
}

void toggleButtonState() {
  cmd = CMD_BUTTON_CHANGE;
}

void initSonoffButton() {
  pinMode(SONOFF_BUTTON, INPUT);
  attachInterrupt(SONOFF_BUTTON, toggleButtonState, CHANGE);
}

