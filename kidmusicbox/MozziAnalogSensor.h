#ifndef MOZZIANALOGSENSOR_H
#define MOZZIANALOGSENSOR_H
#include <Arduino.h>

class MozziAnalogSensor
{
private:
  byte inputPin;
  int fromLow;
  int fromHigh;
  int toLow;
  int toHigh;
  int sensorValue;
  int outputValue;
public:
  MozziAnalogSensor(byte inputPin, int fromLow, int fromHigh, int toLow, int toHigh);
  void init();
  int read();
};

#endif