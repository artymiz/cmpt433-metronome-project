/*
This file compiles into a simple executable that
can be used to test if the buttons are set up 
correctly (by looking at console output).

Complete build in in this directory's Makefile.
*/
#include <stdio.h>
#include <unistd.h>
#include "../utility/GPIO.h"

int main(int argc, char const *argv[])
{
    int BUTTON_DEC_BPM = 45;
    int BUTTON_INC_BPM = 68;
    int BUTTON_DEC_VOL = 69;
    int BUTTON_INC_VOL = 67;
    int BUTTON_PLAY_PAUSE = 66;

    GPIO_pinMode(BUTTON_DEC_BPM, false);
    GPIO_pinMode(BUTTON_INC_BPM, false);
    GPIO_pinMode(BUTTON_DEC_VOL, false);
    GPIO_pinMode(BUTTON_INC_VOL, false);
    GPIO_pinMode(BUTTON_PLAY_PAUSE, false);
    
    while (1)
    {
        sleep(1);
        printf("GPIO %d: %d\n", BUTTON_DEC_BPM, GPIO_getValue(BUTTON_DEC_BPM));
        printf("GPIO %d: %d\n", BUTTON_INC_BPM, GPIO_getValue(BUTTON_INC_BPM));
        printf("GPIO %d: %d\n", BUTTON_DEC_VOL, GPIO_getValue(BUTTON_DEC_VOL));
        printf("GPIO %d: %d\n", BUTTON_INC_VOL, GPIO_getValue(BUTTON_INC_VOL));
        printf("GPIO %d: %d\n", BUTTON_PLAY_PAUSE, GPIO_getValue(BUTTON_PLAY_PAUSE));
        puts("----");
    }
    return 0;
}
