/*
  Buzzer.h - Library for simple buzzer
  Created by Thomas Sears, March 13, 2018
  Released into the public domain.
*/
#ifndef Buzzer_h
#define Buzzer_h

#include "Arduino.h"

class Buzzer
{
  public:
    Buzzer(int pin);
    void buzz(int duration, int freq = 5000);
  private:
    int _pin;
};

#endif