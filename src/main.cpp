// #include <Arduino.h>
// #include <SonicRange.h>
// #include <Wire.h>
// //#include <Adafruit_BMP085.h>
// #include <Adafruit_VL53L0X.h>
// #include <SPI.h>
// #include <SD.h>

// void initializeCard();

// const int ultraTrig = 6;
// const int ultraEcho = 7;
// SonicRange sonic(ultraTrig,ultraEcho);

// const int buzzer = 5;

// //Adafruit_BMP085 bmp;
// Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// float timeOfFlight;

// File fd;
// const uint8_t BUFFER_SIZE = 20;
// char fileName[] = "demoFile.txt"; // SD library only supports up to 8.3 names
// char buff[BUFFER_SIZE+2] = "";  // Added two to allow a 2 char peek for EOF state
// uint8_t index = 0;

// const uint8_t chipSelect = 8;
// const uint8_t cardDetect = 9;

// enum states: uint8_t { NORMAL, E, EO };
// uint8_t state = NORMAL;

// bool alreadyBegan = false;  // SD.begin() misbehaves if not first call

// void setup() {
//     Serial.begin(9600);
//     pinMode(buzzer,OUTPUT);

//     //bmp.begin();
//     lox.begin();

//     pinMode(cardDetect, INPUT);

//     initializeCard();
//       // see if the card is present and can be initialized:
//     if (!SD.begin(chipSelect)) {
//         Serial.println("Card failed, or not present");
//         // don't do anything more:
//         while (1);
//     }
//     Serial.println("card initialized.");
// }

// void loop() {
//     // put your main code here, to run repeatedly:
//     Serial.print(sonic.getTime());
//     Serial.print('\t');
//     Serial.println(sonic.getRange());

// /*     Serial.print("Temperature = ");
//     Serial.print(bmp.readTemperature());
//     Serial.println(" *C");
    
//     Serial.print("Pressure = ");
//     Serial.print(bmp.readPressure());
//     Serial.println(" Pa");
    
//     // Calculate altitude assuming 'standard' barometric
//     // pressure of 1013.25 millibar = 101325 Pascal
//     Serial.print("Altitude = ");
//     Serial.print(bmp.readAltitude());
//     Serial.println(" meters");

//     // you can get a more precise measurement of altitude
//     // if you know the current sea level pressure which will
//     // vary with weather and such. If it is 1015 millibars
//     // that is equal to 101500 Pascals.
//     Serial.print("Real altitude = ");
//     Serial.print(bmp.readAltitude(101500));
//     Serial.println(" meters");
//  */

//     VL53L0X_RangingMeasurementData_t measure;

//     Serial.print("Reading a measurement... ");
//     lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

//     if (measure.RangeStatus != 4) {  // phase failures have incorrect data
//         Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
//     } else {
//         Serial.println(" out of range ");
//     }

//     delay(1000);
// }

// void initializeCard()
// {
//   Serial.print(F("Initializing SD card..."));

//   // Is there even a card?
//   if (!digitalRead(cardDetect))
//   {
//     Serial.println(F("No card detected. Waiting for card."));
//     while (!digitalRead(cardDetect));
//     delay(250); // 'Debounce insertion'
//   }
// /*
//   // Card seems to exist.  begin() returns failure
//   // even if it worked if it's not the first call.
//   if (!SD.begin(chipSelect) && !alreadyBegan)  // begin uses half-speed...
//   {
//     Serial.println(F("Initialization failed!"));
//     initializeCard(); // Possible infinite retry loop is as valid as anything
//   }
//   else
//   {
//     alreadyBegan = true;
//   }
//   Serial.println(F("Initialization done."));

//   Serial.print(fileName);
//   if (SD.exists(fileName))
//   {
//     Serial.println(F(" exists."));
//   }
//   else
//   {
//     Serial.println(F(" doesn't exist. Creating."));
//   }

//   Serial.print("Opening file: ");
//   Serial.println(fileName); */
// }
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SonicRange.h>

const int chipSelect = 8;
char filename[] = "test1.txt";

// initialize the sonic sensor
const int echo = 7;
const int trig = 6;
SonicRange sonic(trig,echo);

long sonicTime;
float sonicDistance;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("Card initialized.");

}

void loop() {
  delay(200);

  // prepare for a round of data collection
  String dataString = "";

  // collect data
  sonicTime = sonic.getTime();
  sonicDistance = sonic.getRange();

  // append data to the output string


  // write string to SD memory
  File dataFile = SD.open(filename, FILE_WRITE);
  Serial.print("Saving to file: ");
  Serial.println(filename);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(sonicTime);
    dataFile.print('\t');
    dataFile.println(sonicDistance);
    dataFile.close();
    // print to the serial port too:
    Serial.print(sonicTime);
    Serial.print('\t');
    Serial.println(sonicDistance);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening file");
  }
}