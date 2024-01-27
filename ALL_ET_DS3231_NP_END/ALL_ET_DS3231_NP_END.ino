#include <HardwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <MCP342x.h> 
#include <WiFi.h>
#include <WiFiManager.h>
#include "ThingSpeak.h" 
#include <PubSubClient.h>
#include <esp_task_wdt.h>
#define WDT_TIMEOUT 1240   // define a 3 seconds WDT (Watch Dog Timer)
int cnt = 100;          //รอบการทำงานก่อนรีเซ็ทระบบ
int Valueset = 350;     //เซ็ทค่าความชื่นที่จะให้วาวล์ทำงาน

uint16_t address =  0x6D ;
MCP342x adc = MCP342x(address);
#include "DHT.h"
#define DHTPIN 21     
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);

#include "ETT_PCF8574.h"
ETT_PCF8574 master_relay(PCF8574_ID_DEV0);                                                        // ET-ESP32-RS485  : Output Relay(PCF8574:ID7)

#define SerialDebug           Serial                                                              // USB Serial(Serial0)
#define RTC_INT_PIN           39                                                                  // ESP32-WROVER :IO39
#define RTC_INT_ACTIVE        LOW
#define RTC_INT_DEACTIVE      HIGH 
#include "ET_DS3231.h"
#define RTC_INT_PIN          39                                                                   // ESP32-WROVER :IO39
ET_DS3231 myRTC;
DateTime myTimeNow;
float UTCOffset = +7.0;    // Your timezone relative to UTC (http://en.wikipedia.org/wiki/UTC_offset)
char daysOfTheWeek[8][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
unsigned long lastSecondTime = 0;

const char* mqtt_server = "broker.netpie.io";                       //mqtt จาก device netpie
const int mqtt_port = 1883;
const char* mqtt_Client = "76a00ba1-8e6e-48a5-a4d7-ad957e356656";
const char* mqtt_username = "g3vfyyBQrm8Z7tMqCHN5U3sGCUmC6xpM";
const char* mqtt_password = "Wgd2sRM7FkzdpMXRkmzWnhW7XnLtms1A";
WiFiClient Client1;
PubSubClient client(Client1);
char msg[100];
int relay = 0 ;
int relay1 = 0 ;
int relay2 = 0 ;
int relay3 = 0 ;

//int period = 1500;
//unsigned long countTime = 0;

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection…");
    if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
      Serial.println("Connected");
      client.subscribe("@msg/status");
      client.subscribe("@msg/status1");
      client.subscribe("@msg/status2");
      client.subscribe("@msg/status3");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("Try again in 5 seconds...");
      delay(5000);
    }
  }
}

void onoff(int relay ,int relay1 , int relay2 ,int relay3) {

  if (relay == 1){
    Serial.println("Turn on the light!");
    master_relay.writePin(RELAY_OUT0_PIN, RELAY_ON);      
    client.publish("@shadow/data/update", "{\"data\" : {\"status\" : \"on\"}}");
  } 
 else if (relay == 0) {
    Serial.println("Turn off the light!");
    master_relay.writePin(RELAY_OUT0_PIN, RELAY_OFF);      
    client.publish("@shadow/data/update", "{\"data\" : {\"status\" : \"off\"}}");
  }  

  if (relay1 == 1){
    Serial.println("Turn on the light!");
    master_relay.writePin(RELAY_OUT1_PIN, RELAY_ON);         
    client.publish("@shadow/data/update", "{\"data\" : {\"status1\" : \"on\"}}");
  } 
 else if (relay1 == 0) {
    Serial.println("Turn off the light!");
    master_relay.writePin(RELAY_OUT1_PIN, RELAY_OFF);        
    client.publish("@shadow/data/update", "{\"data\" : {\"status1\" : \"off\"}}");
  } 

  if (relay2 == 1){
    Serial.println("Turn on the light!");
    master_relay.writePin(RELAY_OUT2_PIN, RELAY_ON);         
    client.publish("@shadow/data/update", "{\"data\" : {\"status2\" : \"on\"}}");
  } 
 else if (relay2 == 0) {
    Serial.println("Turn off the light!");
    master_relay.writePin(RELAY_OUT2_PIN, RELAY_OFF);        
    client.publish("@shadow/data/update", "{\"data\" : {\"status2\" : \"off\"}}");
  } 
    if (relay3 == 1){
    Serial.println("Turn on the light!");
    master_relay.writePin(RELAY_OUT3_PIN, RELAY_ON);         
    client.publish("@shadow/data/update", "{\"data\" : {\"status3\" : \"on\"}}");
  } 
 else if (relay3 == 0) {
    Serial.println("Turn off the light!");
    master_relay.writePin(RELAY_OUT3_PIN, RELAY_OFF);        
    client.publish("@shadow/data/update", "{\"data\" : {\"status3\" : \"off\"}}");
  }
}


void callback(char* topic,byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  String msg;
  for (int i = 0; i < length; i++) {
    msg = msg + (char)payload[i];
  }
   Serial.println(msg);

  if (String(topic) == "@msg/status") {
    if (msg == "on"){
      relay = 1;
      onoff(relay,relay1,relay2,relay3); 
    } 
    else if (msg == "off") {
      relay = 0;
      client.publish("@shadow/data/update", "{\"data\" : {\"status\" : \"off\"}}");
      onoff(relay,relay1,relay2,relay3);
    } 
  }
    if (String(topic) == "@msg/status1") {
    if (msg == "on"){
      relay1 = 1;
      onoff(relay,relay1,relay2,relay3); 
    } 
    else if (msg == "off") {
      relay1 = 0;
      client.publish("@shadow/data/update", "{\"data\" : {\"status1\" : \"off\"}}");
      onoff(relay,relay1,relay2,relay3);
    } 
  }
    if (String(topic) == "@msg/status2") {
    if (msg == "on"){
      relay2 = 1;
      onoff(relay,relay1,relay2,relay3); 
    } 
    else if (msg == "off") {
      relay2 = 0;
      client.publish("@shadow/data/update", "{\"data\" : {\"status2\" : \"off\"}}");
      onoff(relay,relay1,relay2,relay3);
    } 
  }
    if (String(topic) == "@msg/status3") {
    if (msg == "on"){
      relay3 = 1;
      onoff(relay,relay1,relay2,relay3); 
    } 
    else if (msg == "off") {
      relay3 = 0;
      client.publish("@shadow/data/update", "{\"data\" : {\"status3\" : \"off\"}}");
      onoff(relay,relay1,relay2,relay3);
    } 
  }
}



void setup()
{
  Serial.begin(9600);
  dht.begin(); 
  Wire.begin();

  esp_task_wdt_init(WDT_TIMEOUT, true);  // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);  

   WiFiManager wm;
    bool res;
    res = wm.autoConnect("AutoConnectAP","password"); // password protected ap
    if(!res) {
        Serial.println("Failed to connect");
    } 
    else {
        Serial.println("connected...yeey :)");
    }
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

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

  pinMode(RTC_INT_PIN,INPUT_PULLUP);
  
  SerialDebug.begin(115200);
  while(!SerialDebug);
  SerialDebug.println("Initialize DS3231");
  myRTC.begin();
  if(myRTC.lostPower()) 
  {
    SerialDebug.println("RTC lost power, lets set the time!");
    myRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));                                            // Setup RTC from date & time this sketch was compiled
  }
  myRTC.armAlarm1(false);
  myRTC.clearAlarm1();
  myRTC.armAlarm2(false);
  myRTC.clearAlarm2();
  myRTC.setAlarm1(0, 0, 0, 0, DS3231_EVERY_SECOND);                                              // Alarm Every Second
  SerialDebug.println("Initial RTC:DS3231....Complete");
}



void relay_on() {
    Serial.println("                    Relay ON");
    master_relay.writePin(RELAY_OUT0_PIN, RELAY_ON);
    master_relay.writePin(RELAY_OUT1_PIN, RELAY_ON);
    master_relay.writePin(RELAY_OUT2_PIN, RELAY_ON);
    master_relay.writePin(RELAY_OUT3_PIN, RELAY_ON);     
    delay(1200000UL);
}
void relay_off() {
    Serial.println("                    Relay OFF");
    master_relay.writePin(RELAY_OUT0_PIN, RELAY_OFF);
    master_relay.writePin(RELAY_OUT1_PIN, RELAY_OFF);
    master_relay.writePin(RELAY_OUT2_PIN, RELAY_OFF);
    master_relay.writePin(RELAY_OUT3_PIN, RELAY_OFF);  
}



void loop()
{  
  if (!client.connected()) {
    reconnect();
  }
  Serial.print("  cnt = ");
  Serial.println(cnt);

  delay(500); 
  myTimeNow = myRTC.now();
    SerialDebug.print("RTC Time : ");
    SerialDebug.print(daysOfTheWeek[myTimeNow.dayOfTheWeek()]);
    SerialDebug.print(',');
    SerialDebug.print(myTimeNow.day());
    SerialDebug.print('/');
    SerialDebug.print(myTimeNow.month());
    SerialDebug.print('/');
    SerialDebug.print(myTimeNow.year());
    SerialDebug.print(" ");
    SerialDebug.print(myTimeNow.hour());
    SerialDebug.print(':');
    SerialDebug.print(myTimeNow.minute());
    SerialDebug.print(':');
    SerialDebug.print(myTimeNow.second());
    SerialDebug.println();                        
    myRTC.clearAlarm1();

  Serial.println(F("DHTxx test!"));
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

  long value = 0;
  MCP342x::Config status;
  uint8_t err = adc.convertAndRead(MCP342x::channel2, MCP342x::oneShot,
				   MCP342x::resolution12, MCP342x::gain1,100000, value, status);
    int soilMoistureValue = map(value, 0, 4095, 0, 1023);
    Serial.print("Value: ");
    Serial.print(soilMoistureValue);
    String data = "{\"data\": {\"humidity\":" + String(h) + ",\"Temperature\": " + String(t) +  ",\"soilMoistureValue\": " + String(soilMoistureValue) +"}}";

    if ( myTimeNow.hour() >= 6 && myTimeNow.hour() <= 8 )
    {
              if(soilMoistureValue >= Valueset)
              {
                relay_on();
              }
              else if(soilMoistureValue < Valueset)
              { 
                relay_off();
              }

    }
    if ( myTimeNow.hour() >= 15 && myTimeNow.hour() <= 17 )
    {
              if(soilMoistureValue >= Valueset)
              {
                relay_on();
                delay(1000);

              }
              else if(soilMoistureValue < Valueset)
              { 
                relay_off(); 
              }
    }

              else 
              { 
                relay_off(); 
              }
  
  onoff(relay,relay1,relay2,relay3);  
//  String data = "{\"data\": {\"humidity\":" + String(h) + ",\"Temperature\": " + String(t) +  ",\"soilMoistureValue\": " + String(soilMoistureValue) +"}}";
  Serial.println(data);
  data.toCharArray(msg, (data.length() + 1));
  client.publish("@shadow/data/update", msg);
  client.loop();


  if (cnt == 0) {                 // คำสั่งรีเซ็ต ESP
  Serial.println("Reset..");
  ESP.restart();       
  }
  cnt--;

  esp_task_wdt_reset();            // Added to repeatedly reset the Watch Dog Timer
  delay(5000); 
}

