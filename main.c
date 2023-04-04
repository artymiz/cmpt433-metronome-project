#include "button-prototype/Button.h"
#include "button-prototype/ButtonHistory.h"
#include "ticker-prototype/Ticker.h"
#include "ticker-prototype/Audio.h"
#include "ticker-prototype/State.h"
#include "Metronome.h"
#include <pthread.h>
#include <stdbool.h>

#define ON_OFF_GPIO_PIN 66
#define CHANGE_MODE_DELAY 500
#define ON_OFF_HOLD_DELAY 1000

int main()
{
    int gpioPins[NUM_BUTTONS];
    gpioPins[BUTTON_PLAY_PAUSE_SHUTDOWN] = ON_OFF_GPIO_PIN;
    Button_initButtons(gpioPins, NUM_BUTTONS);
    Audio_init();
    State_load();
    Ticker_init();
    Metronome_init();

    //blocking call
    Metronome_mainThread();

    Button_cleanupButtons();
    Audio_cleanup();
    State_store();
    Ticker_cleanup();
    Metronome_cleanup();
    //sleep(1);
    return 0;
}
