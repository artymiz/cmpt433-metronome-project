#include "button-prototype/button.h"
#include "button-prototype/button-history.h"
#include "ticker-prototype/Ticker.h"
#include "ticker-prototype/Audio.h"
#include "ticker-prototype/State.h"
#include <pthread.h>
#include <stdbool.h>

#define ON_OFF_GPIO_PIN 66
#define CHANGE_MODE_DELAY 500
#define ON_OFF_HOLD_DELAY 1000
//every time a button checking thread loops, it waits 20ms
#define DELAY_SHORT 20
//short delay always occurs, so total wait time is DELAY_LONG + DELAY_SHORT
#define PLAY_PAUSE_DELAY_LONG 480
#define TEMPO_CHANGE_DELAY 200
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
    //todo: loop on kill signal instead of 1
    while (1)
    {
        if (isPressed(BUTTON_PLAY_PAUSE_SHUTDOWN)) 
        {
            isPaused = !isPaused;
            if (isPaused)
            {
                bpmBeforePause = State_getBpm();
                State_setBpm(0);
                //return isPaused
                //printf("Pausing, bpm was set to: %d before pausing.\n", State_getBpm());
            }
            else
            {
                State_setBpm(bpmBeforePause);
                //printf("Resuming, bpm is set to: %d\n", State_getBpm());
                //return isPaused (1 or 0)
            }
            delayMs(PLAY_PAUSE_DELAY_LONG);
        }
        if (isLongHeld(BUTTON_PLAY_PAUSE_SHUTDOWN))
        {
            printf("killing program\n");
            //todo: add kill signal to program modules
            //return kill signal (-1?)
            break;
        }
        else if (isShortHeld(BUTTON_PLAY_PAUSE_SHUTDOWN))
        {
            //return change mode (2) (caller can cycle modes whenever this is returned, the current state is irrelevant to this function)
        }
        delayMs(DELAY_SHORT);
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
            State_setBpm(State_getBpm() + 5);
            printf("BPM increased to: %d\n", State_getBpm());
            delayMs(TEMPO_CHANGE_DELAY);
        }
        if (isPressed(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to isPressed(BUTTON_DECREASE_TEMPO)
        {
            State_setBpm(State_getBpm() - 5);
            printf("BPM decreased to: %d\n", State_getBpm());
            delayMs(TEMPO_CHANGE_DELAY);
        }
        delayMs(DELAY_SHORT);
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
            //break;
        }
    }
    return NULL;
}

int main()
{
    int gpioPins[NUM_BUTTONS];
    gpioPins[BUTTON_PLAY_PAUSE_SHUTDOWN] = ON_OFF_GPIO_PIN;
    initButtons(gpioPins, NUM_BUTTONS);
    //set other button timing here, if needed
    setShortHoldDelay(BUTTON_PLAY_PAUSE_SHUTDOWN, CHANGE_MODE_DELAY);
    setLongHoldDelay(BUTTON_PLAY_PAUSE_SHUTDOWN, ON_OFF_HOLD_DELAY);
    
    Audio_init();
    State_read();
    Ticker_init();
    while (1)
    {
        // NOTE: play/pause function will return one of [kill signal(-1), play(1), pause(0), change mode(2)]
        //  RETVAL = call play/pause function
        //  if RETVAL == 2
        //      cycle to next mode
        //  if RETVAL == 1 or 0
        //      set state to paused/last state metronome was in
        //  if RETVAL == -1
        //      terminate program
        //
        // if (State_getMode() == STATE_MODE_PAUSED)
        //     pause ticker playback (if not already paused, basically just set bpm to 0?)
        //     delay for some time (100ms?)
        //     continue
        // if (State_getMode() == STATE_MODE_NORMAL)
        //     call normal state function:
        //          check if tempo+/- was pressed
        //          check if vol+/- was pressed
        //          update state as needed
        //          ***im not really sure how we would adjust the beats per bar here, 
        //             we need 2 more buttons for that i think? might have to ditch volume control?
        // if (State_getMode() == STATE_MODE_RECORDING)
        //     check if [button to record a tick] is pressed
        //     record it and store data
        //     when enough data is stored, send current bpm to state.
        //     BPM should be set to 0 at the start of recording, 
        //     call calculate bpm function
        //     update metronome state
        //          ^metronome displays the rolling total as the bpm is recalculated each button press
        //     after some amount of time of no input (5s?) set to normal mode at recorded bpm/beats per bar?
    }
    pthread_create(&tempoChangerThread, NULL, recordBPMRoutine, NULL);
    sleep(10);
    pthread_cancel(tempoChangerThread);
    pthread_join(tempoChangerThread, NULL);
    Ticker_cleanup();
    State_write();
    Audio_cleanup();
    //sleep(1);
    return 0;
}
