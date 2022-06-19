Our option settings in In the Arduino IDE should look like this:

Flash Mode: DOUT (or maybe DIO)
Flash Frequency: 40MHz
Upload Using: Serial
CPU Frequency: 80MHz
Flash Size: 1M (64K SPIFFS)
Debug Port: Disabled
Debug Level: None
Reset Method: ck
Upload Speed: 115200
Port: Your COM port connected to sonoff

To flash the new software to our SONOFF, we have to start Sonoff in flash mode. Follow this steps process:Connection of USB converter to SONOFFHold down the SONOFF buttonToggle the switch to apply power to the Sonoff circuitThen, we can release the SONOFF buttonAfter SONOFF is in flash mode, we can upload new software.This program is very simple, so it is great for first flashing.