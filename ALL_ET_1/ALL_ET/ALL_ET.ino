#include <Adafruit_ADS1X15.h> 
 #include <SPI.h>
#include <Wire.h>
#include <MCP342x.h>
const int AirValue = 570; //you need to change this value that you had recorded in the air
const int WaterValue = 0; //you need to change this value that you had recorded in the water
int intervals = (AirValue - WaterValue)/3;
// 0x68 is the default address for all MCP342x devices
uint16_t address =  0x6D ;
MCP342x adc = MCP342x(address);

 #include <SPI.h>
 //#include <WiFi.h>
 #include <WiFiManager.h>
 #include "ThingSpeak.h" 
 char thingSpeakAddress[] = "api.thingspeak.com";
 String writeAPIKey = "TNA10YS7RH9WUH1W"; 
//char ssid[] = "WIFI_AOM"; 
//char pass[] = "33262537";
#define WIFI_STA_NAME "RMUTSV_IoT"
#define WIFI_STA_PASS "CoE39201"

 WiFiClient client;





void setup()
{
  Serial.begin(9600);
//  WiFi.begin(ssid, pass);
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
  // ********Check device present
  Wire.requestFrom(address, (uint8_t)1);
  if (!Wire.available()) {
   Serial.print("No device found at address ");
    Serial.println(address, HEX);
    while (1);}
}


void loop()
{  
 delay(1000); 
  long value = 0;
  MCP342x::Config status;
  uint8_t err = adc.convertAndRead(MCP342x::channel1, MCP342x::oneShot,
				   MCP342x::resolution12, MCP342x::gain1,100000, value, status);

    int soilMoistureValue = map(value, 0, 4095, 0, 1023);
    Serial.print("Value: ");
    Serial.print(soilMoistureValue);

    if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
    {
      Serial.println("    Very Wet");
    }
      else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
    {
      Serial.println("    Wet");
    }
      else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
    {
      Serial.println("    Dry");
    }

    String soil_moisture = (String) soilMoistureValue;
    String data = "&field3=" + soil_moisture; 
    Serial.println();
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

}

