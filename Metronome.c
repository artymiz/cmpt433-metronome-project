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
#include "utility/UDPMessageController.h"

#define CHANGE_MODE_DELAY 1000
#define ON_OFF_HOLD_DELAY 3000

// every time a button checking thread loops, it waits 20ms
#define DELAY_SHORT 20
// short delay always occurs, so total wait time is DELAY_LONG + DELAY_SHORT
#define PLAY_PAUSE_DELAY_LONG 480

#define CHANGE_VOL_DELAY_SHORT 1000
#define CHANGE_VOL_DELAY_LONG 2000
#define CHANGE_BPM_DELAY_SHORT 1000
#define CHANGE_BPM_DELAY_LONG 3000

#define WAIT_BEFORE_NEXT_EVENT_MS 200
#define VOL_CHANGE_PRESS 1
#define VOL_CHANGE_SHORT_HOLD 3
#define VOL_CHANGE_LONG_HOLD 5
#define BPM_CHANGE_PRESS 1
#define BPM_CHANGE_SHORT_HOLD 5
#define BPM_CHANGE_LONG_HOLD 10
#define TIMESIG_CHANGE_PRESS 1
#define TIMESIG_CHANGE_SHORT_HOLD 3
#define TIMESIG_CHANGE_LONG_HOLD 5

#define NS_PER_MS 1000000LL

//example usage of configCommand: I want to check if the user pressed the VOL_UP_BUTTON, but I am in recording mode.
//command.button = BUTTON_INC_VOL; 
//command.direction = 1; 
//Metronome_changeStateSetting(command);

static struct configCommand bpmCommandInc;
static struct configCommand bpmCommandDec;
static struct configCommand volCommandInc;
static struct configCommand volCommandDec;
static struct configCommand timeSigCommandInc;
static struct configCommand timeSigCommandDec;
static pthread_t udpListenerThread;
static void* Metronome_runUDPListeningLoop();

void Metronome_changeStateSetting(struct configCommand* command)
{
    int newSettingValue = State_get(command->stateId);
    enum buttons b = command->button;
    int delta = 0;
    if (Button_isLongHeld(b))
        delta = command->deltas[2] * command->direction;
    else if (Button_isShortHeld(b))
        delta = command->deltas[1] * command->direction;
    else if (Button_isPressed(b))
        delta = command->deltas[0] * command->direction;
    newSettingValue += delta;

    if (newSettingValue != State_get(command->stateId))
    {
        long long curTime = getTimeInNanoS();
        if ((curTime - command->lastChangeTimestamp) >= (WAIT_BEFORE_NEXT_EVENT_MS * NS_PER_MS))
        {
            command->lastChangeTimestamp = curTime;
            State_set(command->stateId, newSettingValue);
            printf("Setting %d to %d\n", command->stateId, newSettingValue);
        }
    }
}

void Metronome_init()
{
    UDPListenerInit();
    // set other button timing here, if needed
    Button_setShortHoldDelay(BUTTON_PLAY_PAUSE, CHANGE_MODE_DELAY);
    Button_setLongHoldDelay(BUTTON_PLAY_PAUSE, ON_OFF_HOLD_DELAY);

    timeSigCommandInc.lastChangeTimestamp = 0;
    timeSigCommandInc.button = BUTTON_INC_VOL;
    timeSigCommandInc.stateId = ID_TIMESIG;
    timeSigCommandInc.deltas[0] = TIMESIG_CHANGE_PRESS;
    timeSigCommandInc.deltas[1] = TIMESIG_CHANGE_SHORT_HOLD;
    timeSigCommandInc.deltas[2] = TIMESIG_CHANGE_LONG_HOLD;
    timeSigCommandInc.direction = 1;

    timeSigCommandDec.lastChangeTimestamp = 0;
    timeSigCommandDec.button = BUTTON_DEC_VOL;
    timeSigCommandDec.stateId = ID_TIMESIG;
    timeSigCommandDec.deltas[0] = TIMESIG_CHANGE_PRESS;
    timeSigCommandDec.deltas[1] = TIMESIG_CHANGE_SHORT_HOLD;
    timeSigCommandDec.deltas[2] = TIMESIG_CHANGE_LONG_HOLD;
    timeSigCommandDec.direction = -1;

    volCommandInc.lastChangeTimestamp = 0;
    volCommandInc.button = BUTTON_INC_VOL;
    volCommandInc.stateId = ID_VOLUME;
    volCommandInc.deltas[0] = VOL_CHANGE_PRESS;
    volCommandInc.deltas[1] = VOL_CHANGE_SHORT_HOLD;
    volCommandInc.deltas[2] = VOL_CHANGE_LONG_HOLD;
    volCommandInc.direction = 1;

    volCommandDec.lastChangeTimestamp = 0;
    volCommandDec.button = BUTTON_DEC_VOL;
    volCommandDec.stateId = ID_VOLUME;
    volCommandDec.deltas[0] = VOL_CHANGE_PRESS;
    volCommandDec.deltas[1] = VOL_CHANGE_SHORT_HOLD;
    volCommandDec.deltas[2] = VOL_CHANGE_LONG_HOLD;
    volCommandDec.direction = -1;

    bpmCommandInc.lastChangeTimestamp = 0;
    bpmCommandInc.button = BUTTON_INC_BPM;
    bpmCommandInc.stateId = ID_BPM;
    bpmCommandInc.deltas[0] = BPM_CHANGE_PRESS;
    bpmCommandInc.deltas[1] = BPM_CHANGE_SHORT_HOLD;
    bpmCommandInc.deltas[2] = BPM_CHANGE_LONG_HOLD;
    bpmCommandInc.direction = 1;

    bpmCommandDec.lastChangeTimestamp = 0;
    bpmCommandDec.button = BUTTON_DEC_BPM;
    bpmCommandDec.stateId = ID_BPM;
    bpmCommandDec.deltas[0] = BPM_CHANGE_PRESS;
    bpmCommandDec.deltas[1] = BPM_CHANGE_SHORT_HOLD;
    bpmCommandDec.deltas[2] = BPM_CHANGE_LONG_HOLD;
    bpmCommandDec.direction = -1;

    pthread_create(&udpListenerThread, NULL, &Metronome_runUDPListeningLoop, NULL);

}

void Metronome_cleanup()
{
    // set button delays to invalid state
    // set other button timing here, if needed
    Button_setShortHoldDelay(BUTTON_PLAY_PAUSE, -1);
    Button_setLongHoldDelay(BUTTON_PLAY_PAUSE, -1);
    pthread_join(udpListenerThread, NULL);
    UDPListenerCleanup();
}

/**
 * Check button states of the BUTTON_PLAY_PAUSE (red button) to determine whether to
 * play/pause, mode switch, or shutdown.
 */
static void Metronome_checkPlayPauseButton()
{
    int modeButton = BUTTON_PLAY_PAUSE;
    if (Button_justPressed(modeButton)) // toggle play/pause
    {
        bool isPaused = !State_get(ID_ISPAUSED); // new isPaused value
        State_set(ID_ISPAUSED, isPaused);
        printf("%s\n", isPaused ? "PAUSED" : "PLAYING");
    }
    if (Button_justLongHeld(modeButton)) // shutdown
    {
        printf("Killing program\n");
        KillSignal_shutdown();
    }
    else if (Button_justShortHeld(modeButton)) // toggle alt/normal
    {
        printf("Changing metronome mode\n");
        State_set(ID_MODE, !State_get(ID_MODE));
    }
}

static void Metronome_runNormalMode()
{
    // printf("normal mode\n");
    Metronome_changeStateSetting(&bpmCommandInc);
    Metronome_changeStateSetting(&bpmCommandDec);
    Metronome_changeStateSetting(&volCommandInc);
    Metronome_changeStateSetting(&volCommandDec);
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
    // printf("recording mode\n");
    Metronome_changeStateSetting(&timeSigCommandInc);
    Metronome_changeStateSetting(&timeSigCommandDec);
    delayMs(DELAY_SHORT);
}

void Metronome_mainThread()
{
    while (KillSignal_getIsRunning())
    {
        continue;
        Metronome_checkPlayPauseButton();

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
                //change state in whatever way needed (sentimesig/other ui elements to screen, etc)
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
                //change state in whatever way needed (sentimesig/other ui elements to screen, etc)
            }
        }
        //delayMs(20);
    }
}

static void* Metronome_runUDPListeningLoop() {
    char* message;
    while (KillSignal_getIsRunning()) {
        message = UDPlistenForMessage();
        //printf(message);
        if (strncmp(message, "tempoinc", 9) == 0) {
            if (!State_get(ID_ISPAUSED))
                State_set(ID_BPM, State_get(ID_BPM) + 5);
            memset(message, 0, UDP_PACKET_SIZE);
            snprintf(message, UDP_PACKET_SIZE, "tempo %d", State_get(ID_BPM));
            UDPreturnPacket(message);
        } else if (strncmp(message, "tempodec", 9) == 0) {
            if (!State_get(ID_ISPAUSED))
                State_set(ID_BPM, State_get(ID_BPM) - 5);
            memset(message, 0, UDP_PACKET_SIZE);
            snprintf(message, UDP_PACKET_SIZE, "tempo %d", State_get(ID_BPM));
            UDPreturnPacket(message);
        } else if (strncmp(message, "gettempo", 9) == 0) {
            memset(message, 0, UDP_PACKET_SIZE);
            snprintf(message, UDP_PACKET_SIZE, "tempo %d", State_get(ID_BPM));
            UDPreturnPacket(message);
        }
        memset(message, 0, UDP_PACKET_SIZE);
    }
    return NULL;
}
