#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>

class Button
{
private:
  byte pin;
  byte state;
  byte lastReading;
  boolean pushedState;
  unsigned long lastDebounceTime = 0;
  unsigned long debounceDelay = 20;

public:
  Button(byte pin, boolean pushedState);
  void init();
  void update();
  byte getState();
  bool isPressed();
};

#endif