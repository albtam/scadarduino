#include <SD.h>
#include <SPI.h>
#include <OneWire.h> //Version 2.3.4

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

// Configuration
#define LOOP_TIME_MS        2000

// Analog Pins 
#define SINGLE_BATTERY_PIN A3

// Digital Pins 
//const byte CSPIN                  = 57; // Micro SD card shield
const byte CSPIN                  = 10; // VMA 202 shield
const byte TEMPERATURE_SENSOR_PIN = 7;  // Temperature Sensor

// Variable declarations
String     columnNames = "Date; Single Battery Voltage; Temperature";
String     dataString; // holds the data to be written to the SD card
RTC_DS1307 rtc;
OneWire    ds(TEMPERATURE_SENSOR_PIN);

// Local functions
void writeOnSDcard (String dataString)
{
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (dataFile) // if the file is available, write to it
  {
    dataFile.println(dataString);
    dataFile.close();
  }
 
  else // if the file isn't open, pop up an error
  {
    Serial.println("error opening datalog.csv");
  }
}

float getTemperatureInCelsius() 
{
  float invalid_temperature = -300;
  float temperature         = -500;
  byte  data[9], addr[8];
  // data[] : Data read from the scratchpad
  // addr[] : Address of 1-Wire detected sensor
 
  // Look for the next 1-Wire sensor available
  if (!ds.search(addr)) 
  {
    Serial.println("No sensor found.");
    return invalid_temperature;
  }
  
  // Check the received address
  if (OneWire::crc8(addr, 7) != addr[7]) 
  {
    Serial.println("Invalid address.");
    return invalid_temperature;
  }
 
  // Make sure that the sensor type is DS18B20
  if (addr[0] != 0x28) 
  {
    Serial.println("Invalid sensor.");
    return invalid_temperature;
  }
 
  // Reset 1-Wire bus and select the sensor
  ds.reset();
  ds.select(addr);
  
  // Take the measurement
  ds.write(0x44, 1);
  delay(800);
  
  // Reset 1-Wire bus and read the scratchpad
  ds.reset();
  ds.select(addr);
  ds.write(0xBE);
 
  for (byte i = 0; i < 9; i++) {
    data[i] = ds.read();
  }
   
  // Convert raw temperature into Celsius degree
  temperature = (int16_t) ((data[1] << 8) | data[0]) * 0.0625; 
  
  return temperature;
}

void setup() 
{
  Serial.begin(9600); 

  // --- INITIALIZATION OF SD CARD ---
  Serial.print("Initializing SD card...");
  pinMode(CSPIN, OUTPUT);
  
  if (!SD.begin(CSPIN))   // See if the card is present and can be initialized
  {
    Serial.println("Card failed, or not present");
    return;
  }
  else 
  {
    Serial.println("Card initialized");
    writeOnSDcard(columnNames);
  }

  // --- INITIALIZATION OF RTC ---
  if (! rtc.begin()) 
  {
    Serial.println("Couldn't find RTC");
    return;
  }
  else 
  {
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  

}

void loop() 
{
  // --- BATTERY VOLTAGE ---
  int singleBattery_value = analogRead(SINGLE_BATTERY_PIN);
  
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float singleBattery_voltage = float(singleBattery_value) * (5.00 / 1023.00);

  // --- TEMPERATURE ---
  float temperature = getTemperatureInCelsius();



  
  // --- WRITE ON SD CARD ---

  // Write Time

  // Write Battery Voltages

  // Write Temperature
  
  DateTime now = rtc.now();
  
  dataString  = String("");
  dataString += String(now.day());
  dataString += "/";
  dataString += String(now.month());
  dataString += "/";
  dataString += String(now.year());
  dataString += " ";
  dataString += String(now.hour());
  dataString += ":";
  dataString += String(now.minute());
  dataString += ":";
  dataString += String(now.second());
  dataString += ";";
  dataString += singleBattery_voltage;
  dataString += ";";
  dataString += temperature;
  

  writeOnSDcard(dataString);
  Serial.println(dataString);

  delay(LOOP_TIME_MS); 

}
