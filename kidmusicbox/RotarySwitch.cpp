#include "RotarySwitch.h"
#include <Arduino.h>

RotarySwitch::    RotarySwitch(byte pin, byte numberOfPositions )
{
    this->pin = pin;
    this->numberOfPositions = numberOfPositions;
    position = 0;
    init();
}
void RotarySwitch::init()
{
    pinMode(pin, INPUT);
    update();
}
void RotarySwitch::update()
{
    rawValue = analogRead(pin);
    position =  map(rawValue, 0, 1023, 0, numberOfPositions);
}
byte RotarySwitch::getPosition()
{
    update();
    return position;
}
