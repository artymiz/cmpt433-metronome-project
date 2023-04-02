#include "button-prototype/button.h"
#include "button-prototype/button-history.h"
#include "ticker-prototype/Ticker.h"
#include "ticker-prototype/Audio.h"
#include "ticker-prototype/State.h"
#include <pthread.h>
#include <stdbool.h>

#define ON_OFF_GPIO_PIN 66
#define ON_OFF_HOLD_DELAY 1500
#define PLAY_PAUSE_DELAY_SHORT 20
//short delay always occurs, so total wait time is DELAY_LONG + DELAY_SHORT
#define PLAY_PAUSE_DELAY_LONG 480
pthread_t tempoChangerThread;

static void delayMs(long long ms)
{
    const long long NS_PER_MS = 1000000;
    const long long NS_PER_SECOND = 1000000000;
    long long ns = ms * NS_PER_MS;
    struct timespec ts = {ns / NS_PER_SECOND, ns % NS_PER_SECOND};
    nanosleep(&ts, (struct timespec *)NULL);
}

void *playPauseRoutine(void *args)
{
    int bpmBeforePause = 0;
    bool isPaused = false;
    //todo: loop on kill signal
    while (1)
    {
        if (isHeld(BUTTON_PLAY_PAUSE_SHUTDOWN))
        {
            printf("killing program\n");
            //todo: add kill signal to program modules
            break;
        }
        if (isPressed(BUTTON_PLAY_PAUSE_SHUTDOWN)) 
        {
            isPaused = !isPaused;
            if (isPaused)
            {
                bpmBeforePause = State_get(ID_BPM);
                State_set(ID_BPM, 0);
                printf("Pausing, bpm was set to: %d before pausing.\n", bpmBeforePause);
                delayMs(PLAY_PAUSE_DELAY_LONG);
            }
            else
            {
                State_set(ID_BPM, bpmBeforePause);
                printf("Resuming, bpm is set to: %d\n", State_get(ID_BPM));
                delayMs(PLAY_PAUSE_DELAY_LONG);
            }
        }
        delayMs(PLAY_PAUSE_DELAY_SHORT);
    }
    return NULL;
}

void *tempoChangerRoutine(void *args)
{
    //todo: loop on kill signal
    while (1)
    {
        if (isPressed(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to isPressed(BUTTON_INCREASE_TEMPO)
        {
            int newBpm = State_get(ID_BPM) + 5;
            State_set(ID_BPM, newBpm);
            printf("BPM increased to: %d\n", State_get(ID_BPM));
            delayMs(200);
        }
        if (isHeld(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to isPressed(BUTTON_DECREASE_TEMPO)
        {
            int newBpm = State_get(ID_BPM) - 5;
            State_set(ID_BPM, newBpm);
            printf("BPM decreased to: %d\n", State_get(ID_BPM));
            delayMs(200);
        }
        delayMs(PLAY_PAUSE_DELAY_SHORT);
    }
    return NULL;
}

void *recordBPMRoutine(void* args)
{

    //todo: loop on kill signal
    while (1)
    {
        int bpm = calculateBPM();
        //printf("bpm before record: %d\n", bpm);
        //todo: check if in record mode
        //if (in record mode) {
        recordButtonPress(BUTTON_PLAY_PAUSE_SHUTDOWN); // change to recordButtonPress(BUTTON_INCREASE_TEMPO)
        //}
        delayMs(20);
        if (bpm != -1) 
        {
            printf("bpm after record: %d\n", bpm);
            break;
        }
    }
    return NULL;
}

int main()
{
    int gpioPins[NUM_BUTTONS];
    gpioPins[BUTTON_PLAY_PAUSE_SHUTDOWN] = ON_OFF_GPIO_PIN;
    initButtons(gpioPins, NUM_BUTTONS);
    setHoldDelay(BUTTON_PLAY_PAUSE_SHUTDOWN, ON_OFF_HOLD_DELAY);
    Audio_init();
    State_load();
    Ticker_init();
    pthread_create(&tempoChangerThread, NULL, recordBPMRoutine, NULL);
    sleep(10);
    pthread_cancel(tempoChangerThread);
    pthread_join(tempoChangerThread, NULL);
    Ticker_cleanup();
    State_store();
    Audio_cleanup();
    //sleep(1);
    return 0;
}
