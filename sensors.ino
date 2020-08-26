#include "DHT.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#define DHTPIN 6     // Digital pin connected to the DHT sensor
#define hwPin A0
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);
int hwValue = 0;
int hwPer = 0;

void setup() {
  Serial.begin(9600);


  dht.begin();
}

void loop() {
  delay(2000);
  sensorDHT();
  sensorSoil();
  delay(30000);
}
int convertToPercent(int value)
{
int perValue = 0;
perValue = map(value,1023,5,0,100);
return perValue;

}
void printValuesToSerial(int sensorValue, int percent)
{
Serial.print("\n\nMoisture: ");
Serial.print(sensorValue);
Serial.print("\nPercent: ");
Serial.print(percent);
Serial.println("%");
}
void sensorDHT(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(F(" Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));

}

void sensorSoil(){
    for (int i = 0; i <= 100; i++) 
 { 
   hwValue = hwValue + analogRead(hwPin); 
   delay(1);
 }
  hwPer = convertToPercent(hwValue);
  printValuesToSerial(hwValue, hwPer);
  hwValue = 0;
}
