#include "DHT.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#define DHTPIN 6     // Digital pin connected to the DHT sensor
#define hwPin A0
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define motorPin 9
#define aTX_espRX 2
#define aRX_espTX 3

ESP8266WiFiMulti WiFiMulti;
int motorPin = 9;
String AP = "Theo";      
String PASS = "TheoRoseti";
String HOST = "127.0.0.1";
String PORT = "5000";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
float humidity_soil = 0.0;
float humidity_air = 0.0;
float temperature_air = 0.0;
float hic = 0.0;
int percent = 0.0;
SoftwareSerial esp8266(aTX_espRX, aRX_espTX);

DHT dht(DHTPIN, DHTTYPE);
int hwValue = 0;
int hwPer = 0;

void setup() {
  Serial.begin(9600);
  pinMode(motorPin, OUTPUT);
  WiFi.begin("Theo", "TheoRoseti");
 
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(1000);
    Serial.println("Connecting..");
  }
  
}

void loop() {
  delay(2000);
  sensorDHT();
  sensorSoil();
  delay(30000);
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;  //Declare an object of class HTTPClient
    config_req_Humidity(http);
    config_req_Temperature(htttp);
    onoff_req(http);
    
    StaticJsonBuffer<300> JSONbuffer;
    JsonObject& JSONencoder = JSONbuffer.createObject();
    JSONencoder["sensorType"] = "Temperature";
 
    JsonArray& values = JSONencoder.createNestedArray("values"); //JSON array
    values.add(humidity_air); //Add value to array
    values.add(humidity_soil); //Add value to array
    values.add(temperature_air); //Add value to array
    values.add(hic);
    values.add(hw_per);
    
  delay(10000);    //Send a request every 10 seconds
 
  
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
  humidity_air = dht.readHumidity();
  temperature_air = dht.readTemperature();
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(t, h, false);
  
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

void config_req_Humidity(HTTPClient http){
  int hum_air = 0;
  int hum_soil = 0;
  http.begin("http://127.0.0.1:5000/Humidity"); 

    int httpCode = http.GET(); //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      char* separator = strchr(payload, ',');
    if (separator != 0)
    {
        // Actually split the string in 2: replace ',' with 0
        *separator = 0;
        hum_soil = atoi(command);
        ++separator;
        hum_air = atoi(separator);
        if(hum_soil>humidity_soil){
          digitalWrite(motorPin, HIGH);
        }
        if(hum_soil<humidity_soil){
          digitalWrite(motorPin, LOW);
        }
        if(hum_air<humidity_air){
          digitalWrite(motorPin, HIGH);
        }
        if(hum_air>humidity_air){
          if(hum_soil<humidity_soil){
            digitalWrite(motorPin, HIGH);
          }else {
            digitalWrite(motorPin, LOW);
          }
        }else {
          digitalWrite(motorPin, LOW);
        }
    }else Serial.println("An error ocurred");
    if (hum_soil > 
    http.end();   //Close connection
 
  }
 
  delay(10000); 
}
void config_req_Temperature(HTTPClient htttp){
  int temp = 0;
  http.begin("http://127.0.0.1:5000/Temperature"); 

    int httpCode = http.GET(); //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);             
      temp = atoi(payload);
      if(temp<temperature_air){
        digitalWrite(motorPin, HIGH);
      }else{
        digitalWrite(motorPin, LOW);
      }
    }else Serial.println("An error ocurred");
 
    http.end();   //Close connection
 
  }
 
  delay(10000); 
}
void onoff_req(HTTPClient http){
  int command = 0;
  http.begin("http://127.0.0.1:5000/Manual"); 

    int httpCode = http.GET(); //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
                 
      command = atoi(payload);
      if(command == 1){
        digitalWrite(motorPin, HIGH);
      }else{
        digitalWrite(motorPin, LOW);
      }
    }else Serial.println("An error ocurred");
 
    http.end();   //Close connection
 
  }
 
  delay(10000); 
}
