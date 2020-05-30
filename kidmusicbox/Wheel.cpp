#include "Wheel.h"
#include <Arduino.h>

Wheel::Wheel(byte pin)
{
    this->pin = pin;
    init();
}
void Wheel::init()
{
    pinMode(pin, INPUT);
    update();
}
void Wheel::update()
{
    newWheelState = digitalRead(pin);
    if (newWheelState != lastWheelState)
    {
        wheelTicks++;
    }
    lastWheelState = newWheelState;
}
uint16_t Wheel::read()
{
    uint16_t delta = wheelTicks - lastWheelTicks;
    lastWheelTicks = wheelTicks;
    return delta;
}