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
//every time a button checking thread loops, it waits 20ms
#define DELAY_SHORT 20
//short delay always occurs, so total wait time is DELAY_LONG + DELAY_SHORT
#define PLAY_PAUSE_DELAY_LONG 480
#define TEMPO_CHANGE_DELAY 200
#define METRONOME_KILL_SIGNAL -1
#define METRONOME_PAUSE 1
#define METRONOME_PLAY 0
#define METRONOME_CHANGE_MODE 2
#define METRONOME_NO_CHANGE 3
#define VOL_CHANGE_PRESS 1
#define VOL_CHANGE_SHORT_HOLD 3
#define VOL_CHANGE_LONG_HOLD 5

#define BPM_CHANGE_PRESS 1
#define BPM_CHANGE_SHORT_HOLD 5
#define BPM_CHANGE_LONG_HOLD 10

static int bpmBeforePause;

static void delayMs(long long ms)
{
    const long long NS_PER_MS = 1000000;
    const long long NS_PER_SECOND = 1000000000;
    long long ns = ms * NS_PER_MS;
    struct timespec ts = {ns / NS_PER_SECOND, ns % NS_PER_SECOND};
    nanosleep(&ts, (struct timespec *)NULL);
}

// returns one of:
// METRONOME_PAUSE/METRONOME_PLAY (the REQUESTED state)
// METRONOME_CHANGE_MODE
// METRONOME_KILL_SIGNAL
// METRONOME_NO_CHANGE
// playPauseRoutine DOES NOT change metronome state
int Metronome_playPauseSubroutine()
{
    bool isPaused = State_get(ID_ISPAUSED);
    //return val is only changed if the play/pause/shutdown/changemode button is pressed/held
    int retVal = METRONOME_NO_CHANGE;

    //because Button_isPressed is true until Button_isShortHeld is true, we need make sure the pause signal is sent only once
    if (Button_isPressed(BUTTON_PLAY_PAUSE_SHUTDOWN) && Button_getTimeHeld(BUTTON_PLAY_PAUSE_SHUTDOWN) <= BUTTON_SAMPLE_RATE_MS) 
    {
        isPaused = !isPaused;
        retVal = isPaused;
    }
    if (Button_isLongHeld(BUTTON_PLAY_PAUSE_SHUTDOWN))
    {
        printf("Killing program\n");
        retVal = METRONOME_KILL_SIGNAL;
    }
    else if (Button_isShortHeld(BUTTON_PLAY_PAUSE_SHUTDOWN))
    {
        printf("Changing metronome mode\n");
        retVal = METRONOME_CHANGE_MODE;
    }
    return retVal;
}

void Metronome_normalModeSubroutine(void *args)
{
    Metronome_changeTempo();
    Metronome_changeVolume();
    delayMs(DELAY_SHORT);
}

void Metronome_recordingModeSubroutine(void* args)
{
    int bpm = ButtonHistory_calculateBPM();
    ButtonHistory_recordButtonPress(BUTTON_PLAY_PAUSE_SHUTDOWN); // change to ButtonHistory_recordButtonPress(BUTTON_INCREASE_TEMPO)
    delayMs(20);
    if (bpm != -1) 
    {
        printf("bpm after record: %d\n", bpm);
        //break;
    }
}

void Metronome_mainRoutine()
{
    bool killProgram = false;
    while (!killProgram)
    {
        int retVal = Metronome_playPauseSubroutine();
        switch (retVal)
        {
            case METRONOME_KILL_SIGNAL :
                killProgram = true;
                break;
            case METRONOME_CHANGE_MODE :
                //toggle mode, because we only have 2
                State_set(ID_MODE, !State_get(ID_MODE));
                break;
            case METRONOME_PAUSE :
                bpmBeforePause = State_get(ID_BPM);
                State_set(ID_BPM, 0);
                printf("Pausing, bpm was set to: %d before pausing.\n", bpmBeforePause);
                State_set(ID_ISPAUSED, 1);
                break;
            case METRONOME_PLAY :
                //BPM was changed from outside of this thread, so we throw out bpmBeforePause
                //and assume that the state file was changed by the other thread
                if (State_get(ID_BPM) != 0)
                {
                    bpmBeforePause = 0;
                    break;
                }
                State_set(ID_BPM, bpmBeforePause);
                printf("Resuming, bpm is set to: %d\n", State_get(ID_BPM));
                State_set(ID_ISPAUSED, 0);
                break;
        }
        if (killProgram)
            continue;

        if (State_get(ID_MODE))
        { //MODE = NORMAL
            if (State_get(ID_ISPAUSED))
            {
                //change state in whatever way needed (like send "PAUSED" message to screen)
                delayMs(100);
                continue;
            }
        }
        else 
        { //MODE = RECORDING
            if (State_get(ID_ISPAUSED))
            {
                //change state in whatever way needed (like send "PAUSED" message to screen)
                delayMs(100);
                continue;
            }
        }
    }
}

void Metronome_init()
{
    //set other button timing here, if needed
    Button_setShortHoldDelay(BUTTON_PLAY_PAUSE_SHUTDOWN, CHANGE_MODE_DELAY);
    Button_setLongHoldDelay(BUTTON_PLAY_PAUSE_SHUTDOWN, ON_OFF_HOLD_DELAY);
    //theres a bunch of code in main that needs to be moved here
    bpmBeforePause = 0;
}

void Metronome_cleanup()
{
    bpmBeforePause = 1;
}

void Metronome_changeTempo()
{
    int newBpm = State_get(ID_BPM);
    int delta = 0;
    if (Button_isPressed(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to BUTTON_INCREASE_TEMPO)
        delta = BPM_CHANGE_PRESS;
    else if (Button_isLongHeld(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to BUTTON_INCREASE_TEMPO)
        delta = BPM_CHANGE_SHORT_HOLD;
    else if (Button_isShortHeld(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to BUTTON_INCREASE_TEMPO)
        delta = BPM_CHANGE_LONG_HOLD;

    if (Button_isPressed(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to BUTTON_DECREASE_TEMPO)
        delta = -BPM_CHANGE_PRESS;
    else if (Button_isLongHeld(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to BUTTON_DECREASE_TEMPO)
        delta = -BPM_CHANGE_SHORT_HOLD;
    else if (Button_isShortHeld(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to BUTTON_DECREASE_TEMPO)
        delta = -BPM_CHANGE_LONG_HOLD;
    
    newBpm += delta;
    if (newBpm != State_get(ID_BPM)) 
    {
        State_set(ID_BPM, newBpm);
        printf("BPM changed to %d\n", newBpm);
    }
}

void Metronome_changeVolume()
{
    int newVolume = State_get(ID_VOLUME);
    int delta = 0;

    if (Button_isPressed(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to BUTTON_INCREASE_VOLUME)
        delta = VOL_CHANGE_PRESS;
    else if (Button_isLongHeld(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to BUTTON_INCREASE_VOLUME)
        delta = VOL_CHANGE_SHORT_HOLD;
    else if (Button_isShortHeld(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to BUTTON_INCREASE_VOLUME)
        delta = VOL_CHANGE_LONG_HOLD;

    if (Button_isPressed(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to BUTTON_DECREASE_VOLUME)
        delta = -VOL_CHANGE_PRESS;
    else if (Button_isLongHeld(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to BUTTON_DECREASE_VOLUME)
        delta = -VOL_CHANGE_SHORT_HOLD;
    else if (Button_isShortHeld(BUTTON_PLAY_PAUSE_SHUTDOWN)) // change to BUTTON_DECREASE_VOLUME)
        delta = -VOL_CHANGE_LONG_HOLD;

    newVolume += delta;
    if (newVolume != State_get(ID_VOLUME)) 
    {
        State_set(ID_BPM, newVolume);
        printf("Volume changed to %d\n", newVolume); 
    }
}