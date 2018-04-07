/*
  SonicRange.h - Library for ultrasonic range sensor
  Created by Thomas Sears, March 12, 2018
  Released into the public domain.
*/
#ifndef SonicRange_h
#define SonicRange_h

#include "Arduino.h"

class SonicRange
{
  public:
    SonicRange(int triggerPin, int echoPin);
    long getTime();
    float getRange();
  private:
    int _triggerPin, _echoPin;
    float _time, _range;
    const float _timeToRange = 0.343/2;
};

#endif