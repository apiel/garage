#include "DHT.h"
#define DHTTYPE DHT22
DHT dht(pinDHT22, DHTTYPE, 11);

double prevTemp = 0;
double prevHumidity = 0;

void readTemp() {
 double Temp = dht.readTemperature();
 double Humidity = dht.readHumidity();

 Serial.println("T: " + String(Temp));
 Serial.println("H: " + String(Humidity));

 if (abs(Temp - prevTemp) > 0.5) {
  prevTemp = Temp;
  Serial.println("Temperature changed: " + String(Temp));
 }

 if (abs(Humidity - prevHumidity) > 0.5) {
  prevHumidity = Humidity;
  Serial.println("Humidity changed: " + String(Humidity));
 } 
}
