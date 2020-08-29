#include "DHT.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>

#define DHTPIN 6     // Digital pin connected to the DHT sensor
#define hwPin A0
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define motorPin 9

int sdPin = 4;
File sensorDataDHT;
File sensorDataSoil;
DHT dht(DHTPIN, DHTTYPE);
int hwValue = 0;
int hwPer = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(10, OUTPUT); //reserve pin10 output
  SD.begin(sdPin);
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
Serial.print("\n\nMoisture:");
Serial.print(sensorValue);
Serial.print("\nPercent:");
Serial.print(percent);
///Serial.println("%");
}

void sensorDHT(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  if(SD.exists("DHT.txt")){
    SD.remove("DHT.txt");
  }
  
  sensorDataDHT = SD.open("DHT.txt", FILE_WRITE);
  if(sensorDataDHT){
  Serial.print(F("Humidity:"));
  Serial.println(h);
  Serial.print(F("Temperature:"));
  Serial.println(t);
  Serial.print(F("Heat index:"));
  Serial.println(hic);
  
  delay(250);
  
  sensorDataDHT.print(F("Humidity:"));
  sensorDataDHT.println(h);
  sensorDataDHT.print(F("Temperature:"));
  sensorDataDHT.println(t);
  sensorDataDHT.print(F("Heat index:"));
  sensorDataDHT.println(hic);
  sensorDataDHT.close();
  }
  
}

void sensorSoil(){
if(SD.exists("DHT.txt")){
    SD.remove("DHT.txt");
  }
  
    for (int i = 0; i <= 100; i++) 
 { 
   hwValue = hwValue + analogRead(hwPin); 
   delay(1);
 }
  hwPer = convertToPercent(hwValue);
  sensorDataSoil = SD.open("Soil.txt", FILE_WRITE);
  if(sensorDataSoil){
  printValuesToSerial(hwValue, hwPer);

  delay(250);

  sensorDataSoil.print("\n\nMoisture:");
  sensorDataSoil.print(hwValue);
  sensorDataSoil.print("\nPercent:");
  sensorDataSoil.print(hwPer);
  sensorDataSoil.close();
  }
  hwValue = 0;
}
