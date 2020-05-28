#include "IlluminatedButton.h"


bool IlluminatedButton::isPressed()
{
    return this->button.isPressed();
}
void IlluminatedButton::ledOn()
{
    led.on();
}
void IlluminatedButton::ledOff()
{
    led.off();
}