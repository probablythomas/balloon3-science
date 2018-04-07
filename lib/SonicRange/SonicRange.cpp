#include "Arduino.h"
#include "SonicRange.h"

SonicRange::SonicRange(int triggerPin, int echoPin)
{
    pinMode(triggerPin, OUTPUT);
    _triggerPin = triggerPin;
    pinMode(echoPin, INPUT);
    _echoPin = echoPin;
}

long SonicRange::getTime()
{
    digitalWrite(_triggerPin,LOW);
    delayMicroseconds(2);
    digitalWrite(_triggerPin,HIGH);
    delayMicroseconds(10);
    digitalWrite(_triggerPin,LOW);

    _time = pulseIn(_echoPin,HIGH);

    return _time;
}

float SonicRange::getRange()
{
    _time = SonicRange::getTime();
    _range = _time * _timeToRange;
    return _range;
}