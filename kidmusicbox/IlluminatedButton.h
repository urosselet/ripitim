#ifndef ILLUMINATEDBUTTON_H
#define ILLUMINATEDBUTTON_H
#include <Arduino.h>
#include "Button.h"
#include "Led.h"

class IlluminatedButton
{
private:
  byte buttonPin;
  byte ledPin;
  Button button;
  Led led;

public:
  IlluminatedButton(byte buttonPin, byte ledPin) : button(buttonPin), led(ledPin) {};
  bool isPressed();
  void ledOn();
  void ledOff();
};

#endif