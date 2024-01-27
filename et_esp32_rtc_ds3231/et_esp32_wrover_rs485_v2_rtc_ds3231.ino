/*******************************************************************************
 * ET-ESP32(WROVER) RS485 V2 
 * Tools->Board:"ESP32 Wrover Module"
 *******************************************************************************
 * I2C Interface & I2C Bus
 * -> IO22                = I2C_SCL
 * -> IO21                = I2C_SDA
 * -> I2C RTC:DS3231      = I2C Address : 0x68:1100100(x)
 * -> I2C EEPROM 24LC16   = I2C Address : 0x50:1010000(x)
 * -> I2C ADC MCP3423     = I2C Address : 0x6D:1100101(x)
 * -> I2C Sensor:BME280   = I2C Address : 0x76:1110110(x)
 * -> I2C Sebsor:SHT31    = I2C Address : 0x44:1000100(x)/0x45:1010101(x)
 * SPI Interface SD Card
 * -> SD_CS               = IO4
 * -> SPI_MISO            = IO19
 * -> SPI_MOSI            = IO23
 * -> SPI_SCK             = IO18
 * UART2 RS485 Half Duplex Auto Direction
 * -> IO26                = RX2
 * -> IO27                = TX2
 * User Switch
 * -> IO36                = USER_SW
 * RTC Interrupt
 * -> IO39                = RTC_INT#
 *******************************************************************************/
 
//=================================================================================================
#include <Wire.h> 
//=================================================================================================

//=================================================================================================
// Start of Default Hardware : ET-ESP32(WROVER) RS485 V2
//=================================================================================================
// Remap Pin USART -> C:\Users\Admin\Documents\Arduino\hardware\espressif\esp32\cores\esp32\HardwareSerial.cpp
//                    C:\Users\Admin\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.0\cores\esp32\HardwareSerial.cpp
//=================================================================================================
#include <HardwareSerial.h>
//=================================================================================================
#define SerialDebug           Serial                                                              // USB Serial(Serial0)
//=================================================================================================
#define SerialRS485_RX_PIN    26
#define SerialRS485_TX_PIN    27
#define SerialRS485           Serial2                                                             // Serial2(IO27=TXD,IO26=RXD)
//=================================================================================================
#define SerialLora_RX_PIN     14
#define SerialLora_TX_PIN     13
#define SerialLora            Serial1                                                             // Serial1(IO13=TXD,IO14=RXD)
//=================================================================================================
#define LORA_RES_PIN          33                                                                  // ESP32-WROVER :IO33(LoRa-RESET)
#define LORA_RES_PRESS        LOW
#define LORA_RES_RELEASE      HIGH
//=================================================================================================
#define I2C_SCL_PIN           22                                                                  // ESP32-WROVER : IO22(SCL1)
#define I2C_SDA_PIN           21                                                                  // ESP32-WROVER : IO21(SDA1)
//=================================================================================================
#define LED_PIN               2                                                                   // ESP-WROVER  : IO2
#define LedON                 1
#define LedOFF                0
//=================================================================================================
#define USER_SW_PIN           36                                                                  // ESP32-WROVER :IO36
#define SW_PRESS              LOW
#define SW_RELEASE            HIGH 
//=================================================================================================
#define RTC_INT_PIN           39                                                                  // ESP32-WROVER :IO39
#define RTC_INT_ACTIVE        LOW
#define RTC_INT_DEACTIVE      HIGH 
//=================================================================================================
// End of Default Hardware : ET-ESP32(WROVER) RS485 V2
//=================================================================================================

//=================================================================================================
// ET_DS3231.h
//=================================================================================================
#include "ET_DS3231.h"
//=================================================================================================
#define RTC_INT_PIN          39                                                                   // ESP32-WROVER :IO39
//=================================================================================================
ET_DS3231 myRTC;
DateTime myTimeNow;
//=================================================================================================
float UTCOffset = +7.0;    // Your timezone relative to UTC (http://en.wikipedia.org/wiki/UTC_offset)
char daysOfTheWeek[8][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
//=================================================================================================
unsigned long lastSecondTime = 0;
//=================================================================================================


void setup() 
{
  //===============================================================================================
  // Start of Initial Default Hardware : ET-ESP32(WROVER) RS485 V2
  //===============================================================================================
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LedOFF);
  //===============================================================================================
  pinMode(USER_SW_PIN,INPUT_PULLUP);
  pinMode(RTC_INT_PIN,INPUT_PULLUP);
  //===============================================================================================
  Wire.begin(I2C_SDA_PIN,I2C_SCL_PIN);                                                      
  //===============================================================================================
  SerialDebug.begin(115200);
  while(!SerialDebug);
  //===============================================================================================
  SerialRS485.begin(9600, SERIAL_8N1, SerialRS485_RX_PIN, SerialRS485_TX_PIN);
  while(!SerialRS485);
  //===============================================================================================
  // End of Initial Default Hardware : ET-ESP32(WROVER) RS485 V2
  //===============================================================================================

  //===============================================================================================
  SerialDebug.println();
  SerialDebug.println("ET-ESP32(WROVER)RS485 V2.....Ready");
  //===============================================================================================
  
  //===============================================================================================
  SerialDebug.println();
  SerialDebug.println("================================================================");
  SerialDebug.println();
  SerialDebug.println("Initialize DS3231");
  //===============================================================================================
  myRTC.begin();
  myRTC.adjust(DateTime(__DATE__, __TIME__));  // Time and date is expanded to date and time on your computer at compiletime
  Serial.print('Time and date set');
  //===============================================================================================
  if(myRTC.lostPower()) 
  {
    SerialDebug.println("RTC lost power, lets set the time!");
    myRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));                                            // Setup RTC from date & time this sketch was compiled
  }
}

void loop() 
{
  //===============================================================================================
  if(myRTC.isAlarm1(false))
  {
    //=============================================================================================
    myTimeNow = myRTC.now();
    //=============================================================================================
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
    //=============================================================================================
                          
    //=============================================================================================
    myRTC.clearAlarm1();
    //=============================================================================================
  }
}

