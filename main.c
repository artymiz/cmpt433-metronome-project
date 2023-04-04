#include "button-prototype/Button.h"
#include "button-prototype/ButtonHistory.h"
#include "ticker-prototype/Ticker.h"
#include "ticker-prototype/Audio.h"
#include "ticker-prototype/State.h"
#include "Metronome.h"
#include <pthread.h>
#include <stdbool.h>

#define GPIO_DEC_BPM 45 // p8.11
#define GPIO_INC_BPM 68 // p8.10
#define GPIO_DEC_VOL 69 // p8.9
#define GPIO_INC_VOL 67 // p8.8
#define GPIO_PLAY_PAUSE 66 // p8.7

#define CHANGE_MODE_DELAY 500
#define ON_OFF_HOLD_DELAY 1000

int main()
{
    int gpioPins[NUM_BUTTONS];
    gpioPins[BUTTON_DEC_BPM] = GPIO_DEC_BPM;
    gpioPins[BUTTON_INC_BPM] = GPIO_INC_BPM;
    gpioPins[BUTTON_DEC_VOL] = GPIO_DEC_VOL;
    gpioPins[BUTTON_INC_VOL] = GPIO_INC_VOL;
    gpioPins[BUTTON_PLAY_PAUSE] = GPIO_PLAY_PAUSE;
    Button_initButtons(gpioPins, NUM_BUTTONS);
    //Audio_init();
    State_load();
    //Ticker_init();
    Metronome_init();

    //blocking call
    Metronome_mainThread();

    Button_cleanupButtons();
    //Audio_cleanup();
    //State_store();
    //Ticker_cleanup();
    Metronome_cleanup();
    return 0;
}
