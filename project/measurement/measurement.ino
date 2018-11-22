#include <SD.h>
#include <SPI.h>

// Configuration
#define LOOP_TIME_MS        2000

// Analog Pins 
#define SINGLE_BATTERY_PIN A3

// Digital Pins 
const byte CSPIN          = 53; // SD card shield

// Variable declarations
String        columnNames         = "Timestamp; Single Battery Voltage";
String        dataString          ;// holds the data to be written to the SD card
unsigned long time;

// Local functions
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
  
  Serial.println("Card initialized");
  writeOnSDcard(columnNames);

}

void loop() 
{

  // --- READ SINGLE BATTERY VOLTAGE ---
  int singleBattery_value = analogRead(SINGLE_BATTERY_PIN);
  
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float singleBattery_voltage = float(singleBattery_value) * (5.00 / 1023.00);

  // Print out values
  Serial.print("Battery : ");
  Serial.print(singleBattery_voltage);
  Serial.println(" V");
  
  // --- WRITE ON SD CARD ---
  time = millis();
  dataString = String("");
  dataString += time;
  dataString += ';';
  dataString += singleBattery_voltage;
  writeOnSDcard(dataString);

  delay(LOOP_TIME_MS); 

}



