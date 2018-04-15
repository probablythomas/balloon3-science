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
void initSDCard();
bool sensorInit(bool sensorStart);
void writeToFile(float data, bool sep);
void writeToFile(unsigned long data, bool sep);
void writeSep();

// initialize the sonic sensor
const int echo = 7;
const int trig = 6;
SonicRange sonic(trig,echo);
unsigned long sonicTime;
float sonicDistance;

// initialize the light range sensor
VL53L0X light;
unsigned long lightDistance;

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
uint8_t errorCount = 0;

// SD card info
const int chipSelect = 8;
char filename[] = "DATA0000.CSV";
File dataFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // while (!Serial) {
  //   ; // wait for serial port to connect. Needed for native USB port only
  // }
  Wire.begin();
  initHardware();
  initSDCard();
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
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 1000.0F;
    altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    // ********************************************************

    // *** write to SD memory ***
    dataFile = SD.open(filename, FILE_WRITE);
    if (dataFile) {
      writeToFile(currentMillis,true);
      writeToFile(sonicTime,true);
      writeToFile(sonicDistance,true);
      writeToFile(lightDistance,true);
      writeToFile(a.acceleration.x,true);
      writeToFile(a.acceleration.y,true);
      writeToFile(a.acceleration.z,true);
      writeToFile(g.gyro.x,true);
      writeToFile(g.gyro.y,true);
      writeToFile(g.gyro.z,true);
      writeToFile(m.magnetic.x,true);
      writeToFile(m.magnetic.y,true);
      writeToFile(m.magnetic.z,true);
      writeToFile(temperature,true);
      writeToFile(humidity,true);
      writeToFile(pressure,true);
      writeToFile(altitude,false);
      dataFile.println();
      dataFile.close();
    }
    else {
      Serial.println(F("ERROR WRITING TO FILE"));
      errorCount++;
      if (errorCount > 50)
      {
        return;
      }
    }

    previousMillis = currentMillis;
  }
}

void initHardware()
{
  // *** Light range sensor -- VL53L0X ***
  Serial.print(F("Start VL53L0X: "));
  sensorInit(light.init());
  light.setTimeout(500);
  light.setMeasurementTimingBudget(200000);
  // **************************************

  // *** Attitude sensor -- 9DOF LSM9DS1 ***
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);

  Serial.print(F("Start LSM9DS1: "));
  sensorInit(lsm.begin());
  // ***************************************

  // *** Environment sensor -- BME 280 ***
  Serial.print(F("Start BME280: "));
  sensorInit(bme.begin());
  // **************************************
}

bool sensorInit(bool sensorStart)
{
  if(!sensorStart)
  {
    Serial.println(F("FAILED"));
    while(1);
  }
  else
  {
    Serial.println(F("SUCCESS"));
  }
  return sensorStart;
}

void writeToFile(unsigned long data, bool sep)
{
  dataFile.print(data);
  if(sep) writeSep();
}

void writeToFile(float data, bool sep)
{
  dataFile.print(data);
  if(sep) writeSep();
}

void writeSep()
{
  dataFile.print(',');
}

void initSDCard()
{
    Serial.print(F("Initialize SD card: "));

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println(F("FAILED (or not present)"));
    // don't do anything more:
    while (1);
  }
  Serial.println(F("SUCCESS"));

  // create a new file
  Serial.print(F("Create log file: "));
  for (uint8_t i = 0; i < 256; i++) 
  {
    //filename[4] = (i/1000) % 10 + '0';
    filename[5] = (i/100) % 10 + '0';
    filename[6] = (i/10) % 10 + '0';
    filename[7] = i%10 + '0';
    if (!SD.exists(filename)) 
    {
      dataFile = SD.open(filename, FILE_WRITE); 
      if (dataFile) 
      {
        break;  // leave the loop!
      }
    }
    if (i == 255) 
    {
      Serial.println(F("FAILED"));
      while(1);
    }
  }

  Serial.print(F("SUCCESS -- "));
  Serial.println(filename);
  
  dataFile.print(F("Time(ms),SonicTime(ms),SonicRange(mm),LightRange(mm)"));
  dataFile.print(F(","));
  dataFile.print(F("a_x,a_y,a_z,w_x,w_y,w_z,b_x,b_y,b_z"));  
  dataFile.print(F(","));
  dataFile.println(F("temp,rh,press,alt"));

  dataFile.close();
}