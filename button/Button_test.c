#include <stdio.h>
#include <unistd.h>

#include "Button.h"

#define GPIO_DEC_BPM 45 // p8.11
#define GPIO_INC_BPM 68 // p8.10
#define GPIO_DEC_VOL 69 // p8.9
#define GPIO_INC_VOL 67 // p8.8
#define GPIO_PLAY_PAUSE 66 // p8.7

int main(int argc, char *argv[])
{
    int gpioPins[NUM_BUTTONS];
    gpioPins[BUTTON_DEC_BPM] = GPIO_DEC_BPM;
    gpioPins[BUTTON_INC_BPM] = GPIO_INC_BPM;
    gpioPins[BUTTON_DEC_VOL] = GPIO_DEC_VOL;
    gpioPins[BUTTON_INC_VOL] = GPIO_INC_VOL;
    gpioPins[BUTTON_PLAY_PAUSE] = GPIO_PLAY_PAUSE;
    Button_initButtons(gpioPins, NUM_BUTTONS);
    while (1)
    {
        switch (argv[1][0]) // pressed, short hold, or long hold
        {
        case 'p':
            printf("%d is pressed: %d\n", BUTTON_DEC_BPM, Button_isPressed(BUTTON_DEC_BPM));
            printf("%d is pressed: %d\n", BUTTON_INC_BPM, Button_isPressed(BUTTON_INC_BPM));
            printf("%d is pressed: %d\n", BUTTON_DEC_VOL, Button_isPressed(BUTTON_DEC_VOL));
            printf("%d is pressed: %d\n", BUTTON_INC_VOL, Button_isPressed(BUTTON_INC_VOL));
            printf("%d is pressed: %d\n", BUTTON_PLAY_PAUSE, Button_isPressed(BUTTON_PLAY_PAUSE));
            break;
        case 's':
            printf("%d is short held: %d\n", BUTTON_DEC_BPM, Button_isShortHeld(BUTTON_DEC_BPM));
            printf("%d is short held: %d\n", BUTTON_INC_BPM, Button_isShortHeld(BUTTON_INC_BPM));
            printf("%d is short held: %d\n", BUTTON_DEC_VOL, Button_isShortHeld(BUTTON_DEC_VOL));
            printf("%d is short held: %d\n", BUTTON_INC_VOL, Button_isShortHeld(BUTTON_INC_VOL));
            printf("%d is short held: %d\n", BUTTON_PLAY_PAUSE, Button_isShortHeld(BUTTON_PLAY_PAUSE));
            break;
        case 'l':
            printf("%d is long held: %d\n", BUTTON_DEC_BPM, Button_isLongHeld(BUTTON_DEC_BPM));
            printf("%d is long held: %d\n", BUTTON_INC_BPM, Button_isLongHeld(BUTTON_INC_BPM));
            printf("%d is long held: %d\n", BUTTON_DEC_VOL, Button_isLongHeld(BUTTON_DEC_VOL));
            printf("%d is long held: %d\n", BUTTON_INC_VOL, Button_isLongHeld(BUTTON_INC_VOL));
            printf("%d is long held: %d\n", BUTTON_PLAY_PAUSE, Button_isLongHeld(BUTTON_PLAY_PAUSE));
            break;
        default:
            break;
        }
        sleep(1);
    }
    
    Button_cleanupButtons();
}