#ifndef WHEEL_H
#define WHEEL_H
#include <Arduino.h>

class Wheel
{
private:
  byte pin1;
  byte pin2;
  byte lastState = B00;
  byte newState = B00;
  boolean direction = 0;
  uint16_t wheelTicks = 0;
  uint16_t lastWheelTicks = 0;

public:
  Wheel(byte pin1, byte pin2);
  void init();
  void update();
  uint16_t getSpeed();
  boolean getDirection();
};

#endif
