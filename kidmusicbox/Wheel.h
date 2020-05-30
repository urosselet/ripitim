#ifndef WHEEL_H
#define WHEEL_H
#include <Arduino.h>

class Wheel
{
private:
  byte pin;
  boolean lastWheelState = 0;
  boolean newWheelState = 0;
  uint16_t wheelTicks = 0;
  uint16_t lastWheelTicks = 0;

public:
  Wheel(byte pin);
  void init();
  void update();
  uint16_t read();
};

#endif
