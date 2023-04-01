#include "button-history.h"
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define BPM_BUFFER_SIZE 8
//circular buffer that has the history of each time the button was pressed
static long long buttonPressHistoryNanoS[BPM_BUFFER_SIZE];
static unsigned int historyIndex = 0;
static int bpm = 0;

static long long getTimeInNanoS(void);

//accurate to ~20ms
void recordButtonPress(enum buttons button) 
{
    //note: this has a *very* small chance of recording 2 of the same button press event
    //if the button is pressed between the time that the signal on the GPIO pin is returned true 
    //and time held is not yet updated
    if (isPressed(button) && getTimeHeld(button) <= 20)
    {
        buttonPressHistoryNanoS[historyIndex % 8] = getTimeInNanoS();
        ++historyIndex;
    }
}

//calculates bpm from current buffer and returns it
//returns -1 if history is empty
int calculateBPM() 
{
    if (historyIndex < BPM_BUFFER_SIZE)
    {
        printf("Button timing history is not filled - more data required before caclulating BPM\n");
        return -1;
    }
    const long long NS_PER_MS = 1000000;
    long long sumNanoS = 0;
    for (int i = 0; i < historyIndex - 1; ++i)
    {
        //time of i+1's event - time of i's event = time between 2 presses
        sumNanoS = buttonPressHistoryNanoS[i + 1] - buttonPressHistoryNanoS[i];
    }
    bpm = sumNanoS / historyIndex / NS_PER_MS;
    return bpm;
}

int getBPM() 
{
    return bpm;
}
//clear the button press history to re-record user's input
void clearHistory() 
{
    memset(buttonPressHistoryNanoS, 0, sizeof(*buttonPressHistoryNanoS) * BPM_BUFFER_SIZE);
    historyIndex = 0;
}

void initButtonHistory()
{
    historyIndex = 0;    
    memset(buttonPressHistoryNanoS, 0, sizeof(*buttonPressHistoryNanoS) * BPM_BUFFER_SIZE);
}

// Timing function
static long long getTimeInNanoS(void) 
{
    struct timespec spec;
    clock_gettime(CLOCK_BOOTTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec + seconds * 1000*1000*1000;
	assert(nanoSeconds > 0);
    
    static long long lastTimeHack = 0;
    assert(nanoSeconds > lastTimeHack);
    lastTimeHack = nanoSeconds;

    return nanoSeconds;
}