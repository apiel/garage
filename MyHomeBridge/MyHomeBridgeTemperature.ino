double prevTemp;
double curTemp;

void readTemp() {
 double Temp;
 int val = analogRead(0); 
 Temp = log(((10240000/val) - 10000));
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15;              // Convert Kelvin to Celsius

 if ((Temp - prevTemp) > 0.5) {
  prevTemp = Temp;
  Serial.println("Temperature changed: " + String(Temp));
 }
}

