#include "button-history.h"
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define BPM_BUFFER_SIZE 8
//circular buffer that has the history of each time the button was pressed
static long long buttonPressHistoryNanoS[BPM_BUFFER_SIZE];
static unsigned int nextHistEntryIdx = 0;
static long long bpm = 0;

static long long getTimeInNanoS(void);

//accurate to ~20ms
void recordButtonPress(enum buttons button) 
{
    //note: this has a *very* small chance of recording 2 of the same button press event
    //if the button is pressed between the time that the signal on the GPIO pin is returned true 
    //and time held is not yet updated
    if (isPressed(button) && getTimeHeld(button) <= 20)
    {
        buttonPressHistoryNanoS[nextHistEntryIdx % 8] = getTimeInNanoS();
        printf("TIME OF EVENT: %lld\n", buttonPressHistoryNanoS[nextHistEntryIdx % 8]);
        ++nextHistEntryIdx;
    }
}

//calculates bpm from current buffer and returns it
//returns -1 if history is empty
long long calculateBPM() 
{
    if (nextHistEntryIdx < BPM_BUFFER_SIZE)
    {
        //printf("Button timing history is not filled - more data required before caclulating BPM\n");
        return -1;
    }
    const long long NS_PER_S = 1000000000 * 60LL;
    long long sumNanoS = 0;
    int i = (nextHistEntryIdx - (BPM_BUFFER_SIZE));
    for (int iters = 0; iters < BPM_BUFFER_SIZE - 1; ++iters, ++i)
    {
        //time of i+1's event - time of i's event = time between 2 presses

        printf("BEFORE: %lld\n", sumNanoS);
        sumNanoS += buttonPressHistoryNanoS[(i + 1) % BPM_BUFFER_SIZE] - buttonPressHistoryNanoS[i % BPM_BUFFER_SIZE];
        printf("AFTER: %lld\n", sumNanoS);
    }
    printf("%lld\n", sumNanoS);
    

    long long avgBeatNano = sumNanoS / (BPM_BUFFER_SIZE - 1);
    //ns/min / ns/beat
    bpm = NS_PER_S / avgBeatNano;
    //bpm = ((NS_PER_S) / (sumNanoS / (BPM_BUFFER_SIZE - 1)));
    return bpm;
}

long long getBPM() 
{
    return bpm;
}
//clear the button press history to re-record user's input
void clearHistory() 
{
    memset(buttonPressHistoryNanoS, 0, sizeof(*buttonPressHistoryNanoS) * BPM_BUFFER_SIZE);
    nextHistEntryIdx = 0;
}

void initButtonHistory()
{
    nextHistEntryIdx = 0;    
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