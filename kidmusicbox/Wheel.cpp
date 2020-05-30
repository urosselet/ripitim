#include "Wheel.h"
#include <Arduino.h>

Wheel::Wheel(byte pin1, byte pin2)
{
    this->pin1 = pin1;
    this->pin2 = pin2;
    init();
}
void Wheel::init()
{
    pinMode(pin1, INPUT);
    pinMode(pin2, INPUT);
    update();
}
void Wheel::update()
{
    newState = digitalRead(pin1) | (digitalRead(pin2) << 1);
    if (newState != lastState)
    {
        wheelTicks++;
        if ((lastState == B00 && newState == B10) || (lastState == B10 && newState == B11) || (lastState == B11 && newState == B01) || (lastState == B01 && newState == B00))
        {
            direction = 1;
        }
        else
        {
            direction = 0;
        }
    }

    lastState = newState;
}
uint16_t Wheel::getSpeed()
{
    uint16_t delta = wheelTicks - lastWheelTicks;
    lastWheelTicks = wheelTicks;
    return delta;
}
boolean Wheel::getDirection()
{
    return direction;
}