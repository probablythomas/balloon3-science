#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SonicRange.h>
// #include "Adafruit_VL53L0X.h"
#include <VL53L0X.h>

// initialize the sonic sensor
const int echo = 7;
const int trig = 6;
SonicRange sonic(trig,echo);
long sonicTime;
float sonicDistance;

// initialize the light range sensor
// Adafruit_VL53L0X lox = Adafruit_VL53L0X();
VL53L0X light;
long lightDistance;

// Loop timing
unsigned long WaitTime = 1000;
unsigned long previousMillis = 0;

// SD card info
const int chipSelect = 8;
char filename[] = "DATA0000.CSV";
File dataFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Wire.begin();

  Serial.print(F("Initializing SD card: "));

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Card failed, or not present"));
    // don't do anything more:
    while (1);
  }
  Serial.println(F("Card initialized."));

  // create a new file
  Serial.print(F("Creating new log file: "));
  for (uint8_t i = 0; i < 256; i++) 
  {
    filename[4] = (i/1000) % 10 + '0';
    filename[5] = (i/100) % 10 + '0';
    filename[6] = (i/10) % 10 + '0';
    filename[7] = i%10 + '0';
    if (!SD.exists(filename)) 
    {
      dataFile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  if (!dataFile) {
    Serial.println(F("Failed -- cannot create new file"));
    while (1);
  }

  Serial.print(F("Success -- new filename "));
  Serial.println(filename);
  
  dataFile.print(F("millis,sonicTime,sonicDistance,lightDistance"));
  dataFile.print(F(","));
  dataFile.print(F("a_x,a_y,a_z,w_x,w_y,w_z,b_x,b_y,b_z"));  
  dataFile.print(F(","));
  dataFile.println(F("temp,rh,press,alt"));

  dataFile.close();

  Serial.print(F("Initialize VL53L0X: "));
  light.init();
  light.setTimeout(500);
  // if (!lox.begin()) {
  //   Serial.println(F("FAILED"));
  //   while(1);
  // }
  // else {
    Serial.println(F("SUCCESS"));
  // }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= WaitTime)
  {
    // *** prepare for a round of data collection ***
    // String dataString = "";

    // *** collect data ***
    // 1) sonic range data (science!)
    sonicTime = sonic.getTime();
    sonicDistance = sonic.getRange();

    // 2) light range data (science!)
    lightDistance = light.readRangeSingleMillimeters();
    // VL53L0X_RangingMeasurementData_t measure;
    // lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

    // if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    //   lightDistance = measure.RangeMilliMeter;
    // } else {
    //   lightDistance = 0.0;
    // }

    // 3) attitude telemetry
  

    // 4) temperature, humidity, and pressure telemetry


    // append data to the output string
    //dataString += String(sonicTime);

    // write to SD memory
    dataFile = SD.open(filename, FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
      dataFile.print(sonicTime);
      dataFile.print(',');
      dataFile.print(sonicDistance);
      dataFile.print(',');
      dataFile.println(lightDistance);
      dataFile.close();

    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening file");
    }

    // print to the serial port too:
    // Serial.println(sonicTime);
    // Serial.println(sonicDistance);
    // Serial.println(lightDistance);
  }
}