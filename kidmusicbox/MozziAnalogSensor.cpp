#include "MozziAnalogSensor.h"
#include <Arduino.h>
#include <MozziGuts.h>

MozziAnalogSensor:: MozziAnalogSensor(byte inputPin, int fromLow, int fromHigh, int toLow, int toHigh)
{
    this->inputPin = inputPin;
    this->fromLow = fromLow;
    this->fromHigh = fromHigh;
    this->toLow = toLow;
    this->toHigh = toHigh;
    init();
}
void MozziAnalogSensor::init()
{
    pinMode(inputPin, INPUT);
}

int MozziAnalogSensor::read()
{
    int sensorValue = mozziAnalogRead(inputPin);
    outputValue  = map(sensorValue, fromLow, fromHigh, toLow, toHigh);
    return outputValue;
}
