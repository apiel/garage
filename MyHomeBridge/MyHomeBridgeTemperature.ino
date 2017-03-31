#include "DHT.h"
#define DHTTYPE DHT22
DHT dht(pinDHT22, DHTTYPE, 11);

double prevTemp;
double prevHumidity;

void readTemp() {
 double Temp = dht.readTemperature();
 double Humidity = dht.readHumidity();

 if ((Temp - prevTemp) > 0.5) {
  prevTemp = Temp;
  Serial.println("Temperature changed: " + String(Temp));
 }

 if ((Humidity - prevHumidity) > 0.5) {
  prevHumidity = Humidity;
  Serial.println("Humidity changed: " + String(Humidity));
 } 
}
