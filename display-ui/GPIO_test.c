#include <stdio.h>
#include <unistd.h>
#include "GPIO.h"

int main(int argc, char const *argv[])
{
    int joystickUp = 26;
    GPIO_pinMode(joystickUp, "in");
    for (size_t i = 0; i < 20; i++)
    {
        sleep(1);
        printf("GPIO_getValue(joystickUp) = %d\n", GPIO_getValue(joystickUp));
    }
    return 0;
}
