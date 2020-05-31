#include "Button.h"
#include <Arduino.h>

Button::Button(byte pin, boolean pushedState)
{
    this->pin = pin;
    lastReading = LOW;
    this->pushedState = pushedState;

    init();
}
void Button::init()
{
    pinMode(pin, INPUT_PULLUP);
    update();
}
void Button::update()
{
    byte newReading = digitalRead(pin);

    if (newReading != lastReading)
    {
        lastDebounceTime = millis();
    }
    if (millis() - lastDebounceTime > debounceDelay)
    {
        state = newReading;
    }
    lastReading = newReading;
}
byte Button::getState()
{
    update();
    return state;
}
bool Button::isPressed()
{
    return (getState() == pushedState);
}