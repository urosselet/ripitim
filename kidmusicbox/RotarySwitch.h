#ifndef ROTARYSWITCH_H
#define ROTARYSWITCH_H
#include <Arduino.h>
#include <MozziGuts.h>

class RotarySwitch
{
private:
  byte pin;
  byte numberOfPositions;
  byte offset;
  int position;
  int rawValue;

public:
  RotarySwitch(byte pin, byte numberOfPositions);
  void init();
  void update();
  byte getPosition();
};

#endif