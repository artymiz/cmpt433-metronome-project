#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "button.h"

#define ON_OFF_GPIO_PIN 66

int main(int argc, char const *argv[])
{
    printf("sup?");
    int gpioPins[NUM_BUTTONS];
    gpioPins[BUTTON_ON_OFF] = ON_OFF_GPIO_PIN;
    initButtons(gpioPins, NUM_BUTTONS);
    //sleep(1);
    while(1)
    {
        //printf("is pressed? %d, is held? %d\n", isPressed(BUTTON_ON_OFF), isHeld(BUTTON_ON_OFF));
        for (int i = 0; i < 10000; i++) {
            int x = i * i * i / i / i + 1 - i + i - i;
            x = x - 3;
        }
    }
    return 0;
}

