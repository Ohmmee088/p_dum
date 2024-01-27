 //#include <Adafruit_ADS1X15.h> 
 #include <SPI.h>
 #include <Wire.h>
 #include <MCP342x.h> 
 #include <WiFi.h>
 #include <WiFiManager.h>
 #include "ThingSpeak.h" 
 char thingSpeakAddress[] = "api.thingspeak.com";
 String writeAPIKey = "TNA10YS7RH9WUH1W"; 
//char ssid[] = "WIFI_AOM"; 
//char pass[] = "33262537";
//#define WIFI_STA_NAME "RMUTSV_IoT"
//#define WIFI_STA_PASS "CoE39201"
WiFiClient client;

const int AirValue = 570; 
const int WaterValue = 0; 
int intervals = (AirValue - WaterValue)/3;
uint16_t address =  0x6D ;
MCP342x adc = MCP342x(address);
#include "DHT.h"
#define DHTPIN 21     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

#include "ETT_PCF8574.h"
ETT_PCF8574 master_relay(PCF8574_ID_DEV0);                                                        // ET-ESP32-RS485  : Output Relay(PCF8574:ID7)

void setup()
{
  Serial.begin(9600);
  dht.begin(); 
  Wire.begin();

   WiFiManager wm;
    bool res;
    res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
    } 
    else {
        Serial.println("connected...yeey :)");
    }

  MCP342x::generalCallReset();
  delay(1); // MC342x needs 300us to settle, wait 1ms
  Wire.requestFrom(address, (uint8_t)1);
  if (!Wire.available()) {
   Serial.print("No device found at address ");
    Serial.println(address, HEX);
    while (1);}
  Serial.println(F("DHTxx test!"));

  master_relay.begin(0xFF);
  master_relay.writePin(RELAY_OUT3_PIN, RELAY_OFF);
  master_relay.writePin(RELAY_OUT2_PIN, RELAY_OFF);
  master_relay.writePin(RELAY_OUT1_PIN, RELAY_OFF);
  master_relay.writePin(RELAY_OUT0_PIN, RELAY_OFF);

}


void loop()
{  
  delay(1000); 
  Serial.println(F("DHTxx test!"));
//  dht.begin(); 
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    delay(500);
}

  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F(" C "));
  Serial.print(f);
  Serial.print(F(" F  Heat index: "));
  Serial.print(hic);
  Serial.print(F(" C "));
  Serial.print(hif);
  Serial.println(F(" F"));
//delay(500);


  long value = 0;
  MCP342x::Config status;
  uint8_t err = adc.convertAndRead(MCP342x::channel2, MCP342x::oneShot,
				   MCP342x::resolution12, MCP342x::gain1,100000, value, status);

    int soilMoistureValue = map(value, 0, 4095, 0, 1023);
    Serial.print("Value: ");
    Serial.println(soilMoistureValue);

    if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
    {
      Serial.println("Very Wet");
    }

      else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
    {
      Serial.println("Wet");
    }

      else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
    {
      Serial.println("Dry");      
    }


    if(soilMoistureValue >= 300)
    {
      Serial.println("Relay ON");
      master_relay.writePin(RELAY_OUT0_PIN, RELAY_ON);
      master_relay.writePin(RELAY_OUT1_PIN, RELAY_ON);
      master_relay.writePin(RELAY_OUT2_PIN, RELAY_ON);
      master_relay.writePin(RELAY_OUT3_PIN, RELAY_ON);     
//      delay(1200000UL);
    }
      else if(soilMoistureValue < 300)
   { 
      Serial.println("Relay OFF");
      master_relay.writePin(RELAY_OUT0_PIN, RELAY_OFF);
      master_relay.writePin(RELAY_OUT1_PIN, RELAY_OFF);
      master_relay.writePin(RELAY_OUT2_PIN, RELAY_OFF);
      master_relay.writePin(RELAY_OUT3_PIN, RELAY_OFF); 
   }

    String Humidity    = (String) h; 
    String Temperature = (String) t;
    String soil_moisture = (String) soilMoistureValue;
    String data = "field1=" + Humidity 
    +"&field2=" + Temperature 
    + "&field3=" + soil_moisture; 
  if (client.connect(thingSpeakAddress, 80)) {
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(data.length());
    client.print("\n\n");
    client.print(data); // ข้อมูล
   }
Serial.println();
delay(8000); 
//delay(1000); 
//return;
}

