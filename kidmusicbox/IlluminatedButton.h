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
  IlluminatedButton(byte buttonPin, byte ledPin, boolean pushedState) : button(buttonPin, pushedState), led(ledPin){};
  bool isPressed();
  void ledOn(byte intensity);
  void ledOff();
};

#endif