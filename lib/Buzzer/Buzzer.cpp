#include "Arduino.h"
#include "Buzzer.h"

Buzzer::Buzzer(int pin)
{
    pinMode(pin, OUTPUT);
    _pin = pin;
}

void Buzzer::buzz(int duration, int freq =5000)
{
    tone(_pin,freq);
    delay(duration);
    noTone(_pin);
}