#include "button-prototype/Button.h"
#include "button-prototype/ButtonHistory.h"
#include "ticker-prototype/Ticker.h"
#include "ticker-prototype/Audio.h"
#include "ticker-prototype/State.h"
#include "Metronome.h"
#include "utility/KillSignal.h"
#include "utility/Timing.h"
#include <pthread.h>
#include <stdbool.h>

#define ON_OFF_GPIO_PIN 66
#define CHANGE_MODE_DELAY 1000
#define ON_OFF_HOLD_DELAY 3000

//every time a button checking thread loops, it waits 20ms
#define DELAY_SHORT 20
//short delay always occurs, so total wait time is DELAY_LONG + DELAY_SHORT
#define PLAY_PAUSE_DELAY_LONG 480

#define CHANGE_VOL_DELAY_SHORT 1000
#define CHANGE_VOL_DELAY_LONG 2000
#define CHANGE_BPM_DELAY_SHORT 1000
#define CHANGE_BPM_DELAY_LONG 2000

#define WAIT_BEFORE_NEXT_EVENT_MS 200
#define VOL_CHANGE_PRESS 1
#define VOL_CHANGE_SHORT_HOLD 3
#define VOL_CHANGE_LONG_HOLD 5
#define BPM_CHANGE_PRESS 1
#define BPM_CHANGE_SHORT_HOLD 5
#define BPM_CHANGE_LONG_HOLD 30
#define TIMESIG_CHANGE_PRESS 1
#define TIMESIG_CHANGE_SHORT_HOLD 3
#define TIMESIG_CHANGE_LONG_HOLD 5

#define NS_PER_MS 1000000LL

static long long timeVolumeLastChanged;
static long long timeTempoLastChanged;
static int volDeltas[3]     = { VOL_CHANGE_PRESS,       VOL_CHANGE_SHORT_HOLD,      VOL_CHANGE_LONG_HOLD };
static int BPMDeltas[3]     = { BPM_CHANGE_PRESS,       BPM_CHANGE_SHORT_HOLD,      BPM_CHANGE_LONG_HOLD };
static int timeSigDeltas[3] = { TIMESIG_CHANGE_PRESS,   TIMESIG_CHANGE_SHORT_HOLD,  TIMESIG_CHANGE_LONG_HOLD };

//alters MODE and ISPAUSED state switches if user presses the PLAY_PAUSE_SHUTDOWN button
//does nothing if user did not press the big red button
static void Metronome_handleModeButton();
static void Metronome_runNormalMode();
static void Metronome_runRecordingMode();

static void Metronome_handleModeButton()
{
    int modeButton = BUTTON_PLAY_PAUSE_SHUTDOWN;
    if (Button_isPressed(modeButton))
    {
        printf("pressed\n");
        State_set(ID_ISPAUSED, !State_get(ID_ISPAUSED));
    }
    if (Button_isLongHeld(modeButton))
    {
        printf("Killing program\n");
        KillSignal_shutdown();

    }
    else if (Button_isShortHeld(modeButton))
    {
        printf("Changing metronome mode\n");
        State_set(ID_MODE, !State_get(ID_MODE));
    }
}

static void Metronome_runNormalMode()
{
    Metronome_changeTempo();
    Metronome_changeVolume();
    //printf("normal mode\n");
    delayMs(DELAY_SHORT);
}

static void Metronome_runRecordingMode()
{
    //int bpm = ButtonHistory_calculateBPM();
    //ButtonHistory_recordButtonPress(BUTTON_PLAY_PAUSE_SHUTDOWN); // change to ButtonHistory_recordButtonPress(BUTTON_INCREASE_TEMPO)
    //delayMs(20);
    //if (bpm != -1) 
    //{
    //    printf("bpm after record: %d\n", bpm);
        //break;
    //}
    printf("recording mode\n");
    delayMs(DELAY_SHORT);
}

void Metronome_mainThread()
{
    while (KillSignal_getIsRunning())
    {
        //Metronome_handleModeButton();

        if (!KillSignal_getIsRunning())
            break;

        if (State_get(ID_MODE))
        { //MODE = NORMAL
            if (State_get(ID_ISPAUSED))
            {
                //change state in whatever way needed (like send "PAUSED" message to screen)
                //State_set(ID_ISPAUSED, !State_get(ID_ISPAUSED));
                //printf("normal mode paused\n");
                delayMs(100);
                continue;
            }
            else 
            {
                //change state in whatever way needed (send bpm/other ui elements to screen, etc)
                //State_set(ID_ISPAUSED, !State_get(ID_ISPAUSED));
                Metronome_runNormalMode();
            }
        }
        else 
        { //MODE = RECORDING
            if (State_get(ID_ISPAUSED))
            {
                //change state in whatever way needed (like send "PAUSED" message to screen)
                //State_set(ID_ISPAUSED, !State_get(ID_ISPAUSED));
                //printf("recording mode paused\n");
                delayMs(100);
                continue;
            }
            else 
            {
                //State_set(ID_ISPAUSED, !State_get(ID_ISPAUSED));
                Metronome_runRecordingMode();
                //change state in whatever way needed (send bpm/other ui elements to screen, etc)
            }
        }
        //delayMs(20);
    }
}

void Metronome_init()
{
    //set other button timing here, if needed
    Button_setShortHoldDelay(BUTTON_PLAY_PAUSE_SHUTDOWN, CHANGE_MODE_DELAY);
    Button_setLongHoldDelay(BUTTON_PLAY_PAUSE_SHUTDOWN, ON_OFF_HOLD_DELAY);
    Metronome_handleModeButton();
}

void Metronome_cleanup()
{
    //set button delays to invalid state
    //set other button timing here, if needed
    Button_setShortHoldDelay(BUTTON_PLAY_PAUSE_SHUTDOWN, -1);
    Button_setLongHoldDelay(BUTTON_PLAY_PAUSE_SHUTDOWN, -1);
}
//button, [deltas], ID_{state element to change}
void Metronome_changeVolumeBpmOrTimeSignature(enum buttons button, int deltas[3], enum stateid stateId)
{

    int newSetting = State_get(stateId);
    int multiplier = newSetting == BUTTON_PLAY_PAUSE_SHUTDOWN;
    int delta = 0;
    if (Button_isLongHeld(button)) // change to BUTTON_INCREASE_VOLUME)
        delta = deltas[2];
    else if (Button_isShortHeld(button)) // change to BUTTON_INCREASE_VOLUME)
        delta = deltas[1];
    else if (Button_isPressed(button)) // change to BUTTON_INCREASE_VOLUME)
        delta = deltas[0];
    newSetting += delta;
    if (newSetting != State_get(ID_VOLUME)) 
    {
        long long curTime = getTimeInNanoS();
        if ((curTime - timeVolumeLastChanged) >= (WAIT_BEFORE_NEXT_EVENT_MS * NS_PER_MS))
        {
            timeVolumeLastChanged = curTime;
            State_set(ID_VOLUME, newSetting);
            printf("Volume changed to %d\n", State_get(ID_VOLUME));
        }
    }
}