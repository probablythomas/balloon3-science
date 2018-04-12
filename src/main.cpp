#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SonicRange.h>
#include <VL53L0X.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// function declarations
void initHardware();

// initialize the sonic sensor
const int echo = 7;
const int trig = 6;
SonicRange sonic(trig,echo);
long sonicTime;
float sonicDistance;

// initialize the light range sensor
// Adafruit_VL53L0X lox = Adafruit_VL53L0X();
VL53L0X light;
unsigned int lightDistance;

// initialize the lsm9ds1 attitude sensor
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();

// initialize the bme280 environment sensor
Adafruit_BME280 bme;
float temperature;
float pressure;
float altitude;
float humidity;

// Loop timing
const unsigned long WaitTime = 1000;
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

  initHardware();

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
    // *** collect data ***
    // 1) sonic range data (science!)
    sonicTime = sonic.getTime();
    sonicDistance = sonic.getRange();

    // 2) light range data (science!)
    lightDistance = light.readRangeSingleMillimeters();

    // 3) attitude telemetry
    lsm.read();
    sensors_event_t a, m, g, temp;
    lsm.getEvent(&a, &m, &g, &temp); 

    // 4) temperature, humidity, and pressure telemetry
    temperature = bme.readTemperature();
    pressure = bme.readPressure() / 1000.0F;
    altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    humidity = bme.readHumidity();

    // *** write to SD memory ***
    dataFile = SD.open(filename, FILE_WRITE);
    if (dataFile) {
      dataFile.print(currentMillis);
      dataFile.print(',');
      dataFile.print(sonicTime);
      dataFile.print(',');
      dataFile.print(sonicDistance);
      dataFile.print(',');
      dataFile.print(lightDistance);
      dataFile.print(',');
      dataFile.print(a.acceleration.x);
      dataFile.print(',');
      dataFile.print(a.acceleration.y);
      dataFile.print(',');
      dataFile.print(a.acceleration.z);
      dataFile.print(',');
      dataFile.print(g.gyro.x);
      dataFile.print(',');
      dataFile.print(g.gyro.y);
      dataFile.print(',');
      dataFile.print(g.gyro.z);
      dataFile.print(',');
      dataFile.print(m.magnetic.x);
      dataFile.print(',');
      dataFile.print(m.magnetic.y);
      dataFile.print(',');
      dataFile.print(m.magnetic.z);
      dataFile.print(',');
      dataFile.print(temperature);
      dataFile.print(',');
      dataFile.print(humidity);
      dataFile.print(',');
      dataFile.print(pressure);
      dataFile.print(',');
      dataFile.print(altitude);
      dataFile.println();
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

void initHardware()
{
  // *** Attitude sensor -- 9DOF LSM9DS1 ***
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);

  // Try to initialise and warn if we couldn't detect the chip
  Serial.print(F("Start LSM9DS1: "));
  if (!lsm.begin())
  {
    Serial.println(F("FAILED"));
    while (1);
  }
  Serial.println(F("SUCCESS"));
  // ***************************************

  // *** Environment sensor -- BME 280 ***
  Serial.print(F("Start BME280: "));
  if (!bme.begin()) {
    Serial.println(F("FAILED"));
    while (1);
  }
  Serial.println(F("SUCCESS"));
  // **************************************
}