#include "button-prototype/button.h"
#include "ticker-prototype/Ticker.h"
#include "ticker-prototype/Audio.h"
#include "ticker-prototype/State.h"
#include <pthread.h>

#define ON_OFF_GPIO_PIN 66

pthread_t tempoChangerThread;

void *tempoChangerRoutine(void *args)
{
    while (1)
    {
        char ch = getchar();
        if (ch == 'w')
            State_setBpm(State_getBpm() + 5);
        else if (ch == 's')
            State_setBpm(State_getBpm() - 5);
    }
    return NULL;
}

int main()
{
    int gpioPins[NUM_BUTTONS];
    gpioPins[BUTTON_ON_OFF] = ON_OFF_GPIO_PIN;
    initButtons(gpioPins, NUM_BUTTONS);
    Audio_init();
    State_read();
    Ticker_init();
    pthread_create(&tempoChangerThread, NULL, tempoChangerRoutine, NULL);
    sleep(10);
    pthread_cancel(tempoChangerThread);
    pthread_join(tempoChangerThread, NULL);
    Ticker_cleanup();
    State_write();
    Audio_cleanup();
    //sleep(1);
    return 0;
}