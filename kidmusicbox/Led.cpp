#include "Led.h"
#include <Arduino.h>

Led::Led(byte pin)
{
    this->pin = pin;
    init();
}
void Led::init()
{
    pinMode(pin, OUTPUT);
    off();
}
void Led::on(byte intensity)
{
    analogWrite(pin, intensity);
}
void Led::off()
{
    Led::on(0);
}