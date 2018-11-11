#include <SD.h>
#include <SPI.h>
#include <OneWire.h>

/* --- Configuration --- */
#define LOOP_TIME_MS        2000
#define NUMBER_OF_BATTERIES 2

/* --- Analog Pins --- */
#define ALL_BATTERY_PIN          A5
#define SINGLE_BATTERY1_PIN      A3
#define SINGLE_BATTERY2_PIN      A4

/* --- Digital Pins --- */
const byte BROCHE_ONEWIRE = 31; // Temperature Sensor
const byte CSPIN          = 53; // SD card shield

/* --- Variable declaration --- */
String        columnNames         = "Timestamp; Single Battery1 Voltage; Single Battery1 Voltage; Total Battery Voltage; Total Battery Level; Temperature ";
String        dataString          ;// holds the data to be written to the SD card
OneWire       ds(BROCHE_ONEWIRE);    
unsigned long time;


/* --- Function declaration (TO MOVE TO HEADER FILE --- */ 
float getTotalBatteryVoltage();
int computeBatteryLevel(int numberOfBatteries,float totalBattery_voltage);
float getTemperature();
void writeOnSDcard(String dataString);

void setup() 
{
  Serial.begin(9600); 

  /* --- INITIALIZATION OF SD CARD --- */
  Serial.print("Initializing SD card...");
  pinMode(CSPIN, OUTPUT);
  
  if (!SD.begin(CSPIN))   // See if the card is present and can be initialized
  {
    Serial.println("Card failed, or not present");
    return;
  }
  
  Serial.println("Card initialized");
  writeOnSDcard(columnNames);

}

void loop() 
{
  /* --- Variable initialization --- */
  float totalBattery_voltage = 0.0;
  float batteryLevel         = 0.0;
  float temperature          = 0.0;

  /* --- READ SINGLE BATTERY VOLTAGES --- */
  int singleBattery1_value = analogRead(SINGLE_BATTERY1_PIN);
  int singleBattery2_value = analogRead(SINGLE_BATTERY2_PIN);  
  
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float singleBattery1_voltage = float(singleBattery1_value) * (5.00 / 1023.00);
  float singleBattery2_voltage = float(singleBattery2_value) * (5.0 / 1023.0);

  // Print out values
  Serial.print("Battery 1: ");
  Serial.print(singleBattery1_voltage);
  Serial.println(" V");

  Serial.print("Battery 2: ");
  Serial.print(singleBattery2_voltage);
  Serial.println(" V");
  

  /* --- READ TOTAL BATTERY VOLTAGE --- */
  totalBattery_voltage = getTotalBatteryVoltage();

  /* --- COMPUTE BATTERY LEVEL --- */
  batteryLevel = computeBatteryLevel(24, totalBattery_voltage);

  /* --- READ OUTDOOR TEMPERATURE --- */
  //temperature = getTemperature();

  /* --- WRITE ON SD CARD --- */
  time = millis();
  dataString = String("");
  dataString += time;
  dataString += ';';
  dataString += singleBattery1_voltage;
  dataString += ';';
  dataString += singleBattery2_voltage;
  dataString += ';';
  dataString += totalBattery_voltage;
  dataString += ';';
  dataString += batteryLevel;
  dataString += ';';
  dataString += temperature;
  writeOnSDcard(dataString);

  delay(LOOP_TIME_MS); 

}

/* ----------------------- LOCAL FUNCTIONS ----------------------- */
float getTotalBatteryVoltage()
{
  float totalBattery_voltage = 0.0;
  totalBattery_voltage = float(analogRead(ALL_BATTERY_PIN)) * (5.00 / 1023.00);

  Serial.print("TotalBattery_voltage (V): ");
  Serial.println(totalBattery_voltage);
  
  return totalBattery_voltage;
}

/* --------------------------------------------------------------- */
int computeBatteryLevel(int numberOfBatteries, float totalBattery_voltage)
{
  int totalBattery_charge = 0;
  int batteryLevel = 0;
  
  const float charge10_v = 44.0;
  const float charge30_v = 46.8;
  const float charge50_v = 50.0;
  const float charge70_v = 51.2;
  const float charge99_v = 52.0; 

  if (totalBattery_voltage <= charge10_v)
  {
    return batteryLevel = 10;
  }
  else if (totalBattery_charge > charge10_v && totalBattery_charge <= charge30_v)
  {
    return batteryLevel = 30;
  }
  else if (totalBattery_charge > charge30_v && totalBattery_charge <= charge50_v)
  {
    return batteryLevel = 50;
  }
  else if (totalBattery_charge > charge50_v && totalBattery_charge <= charge70_v)
  {
    return batteryLevel = 70;
  }
  else if (totalBattery_charge > charge70_v)
  {
    return batteryLevel = 100;
  }
  else
  {
    return batteryLevel = -1;
  }
}

/* --------------------------------------------------------------- */
//float getTemperature()
//{
//  byte i;
//  byte data[12];
//  byte addr[8];
//  float temp = 0.0;
//
//  ds.search(addr);
//
//
//  if (OneWire::crc8(addr, 7) != addr[7])
//  {
//    return INVALID_ADDRESS;
//  }
//
//  if (addr[0] != 0x28)
//  {
//    return INVALID_SENSOR;
//  }
//
//  ds.reset();
//  ds.select(addr);
//
//  ds.write(0x44);
//  delay(800);
//  ds.reset();
//  ds.select(addr);
//  ds.write(0xBE);
//
//  for (byte i = 0; i < 9; i++)
//  {
//    data[i] = ds.read();
//  }
//
//  temp = (int16_t)((data[1] << 8) + data[0]) * 0.0625;
//
//  return temp;
//  
//
//}

/* --------------------------------------------------------------- */
void writeOnSDcard (String dataString)
{
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (dataFile) // if the file is available, write to it
  {
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString);// only for debugging
  }
 
  else // if the file isn't open, pop up an error
  {
    Serial.println("error opening datalog.csv");
  }
}
