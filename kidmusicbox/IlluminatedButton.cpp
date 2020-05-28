#include "IlluminatedButton.h"

bool IlluminatedButton::isPressed()
{
    return this->button.isPressed();
}
void IlluminatedButton::ledOn(byte intensity)
{
    led.on(intensity);
}
void IlluminatedButton::ledOff()
{
    led.off();
}