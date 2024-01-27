 #include <SPI.h>
 #include <WiFi.h>
 #include "ThingSpeak.h" 

#include "DHT.h"
#define DHTPIN 26 
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);
 const int AirValue = 570; 
const int WaterValue = 0; 
int intervals = (AirValue - WaterValue)/3;
int soilMoistureValue1 ;

char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "IVIYSB8U6PW0FEZZ"; 
#define WIFI_STA_NAME "LAPTOP-010"
#define WIFI_STA_PASS "33262537"

int analogpin = 14;



void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_STA_NAME);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

Serial.println(F("DHTxx test!"));
dht.begin();
}
  
 
void loop() {
  soilMoistureValue1 = analogRead(analogpin); 
//Serial.println(soilMoistureValue);
  int soilMoistureValue = map(soilMoistureValue1, 0, 4095, 0, 1023);     
  Serial.println(soilMoistureValue);
if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
{ Serial.println("Very Wet"); }
else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
{ Serial.println("Wet");  }
else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
{ Serial.println("Dry");  }
delay(1000);


float h = dht.readHumidity();
float t = dht.readTemperature();
float f = dht.readTemperature(true);
if (isnan(h) || isnan(t) || isnan(f)) {
Serial.println(F("Failed to read from DHT sensor!"));
return;
}
float hif = dht.computeHeatIndex(f, h);
float hic = dht.computeHeatIndex(t, h, false);
Serial.print(F("Humidity: "));
Serial.print(h);
Serial.print(F("% Temperature: "));
Serial.print(t);
Serial.print(F(" C "));
Serial.print(f);
Serial.print(F(" F Heat index: "));
Serial.print(hic);
Serial.print(F(" C "));
Serial.print(hif);
Serial.println(F(" F"));
delay(1000);
Serial.println();

}