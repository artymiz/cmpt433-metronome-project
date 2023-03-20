#include <stdio.h>
#include "GPIO.h"
#include "../Timing/Timing.h"

int main(int argc, char const *argv[])
{
    gpioInfo_t joystickUp = {.pin={.header=8, .number=14}, .gpioNumber=26};
    GPIO_usePin(&joystickUp, "in");
    for (size_t i = 0; i < 20; i++)
    {
        Timing_delayMs(500);
        printf("GPIO_getValue(&joystickUp) = %d\n", GPIO_getValue(&joystickUp));
    }
    return 0;
}
