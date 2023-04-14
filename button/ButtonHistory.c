#include "ButtonHistory.h"
#include "../utility/Timing.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define BPM_BUFFER_SIZE 8
//circular buffer that has the history of each time the button was pressed
static long long buttonPressHistoryNanoS[BPM_BUFFER_SIZE];
static unsigned int nextHistEntryIdx = 0;
static long long bpm = 0;

void ButtonHistory_recordButtonPress(enum buttons button) 
{
    if (Button_isPressed(button) && Button_getTimeHeld(button) <= BUTTON_SAMPLE_RATE_MS)
    {
        buttonPressHistoryNanoS[nextHistEntryIdx % 8] = getTimeInNanoS();
        printf("TIME OF EVENT: %lld\n", buttonPressHistoryNanoS[nextHistEntryIdx % 8]);
        ++nextHistEntryIdx;
    }
}

long long ButtonHistory_calculateBPM() 
{
    if (nextHistEntryIdx < BPM_BUFFER_SIZE)
    {
        //printf("Button timing history is not filled - more data required before caclulating BPM\n");
        return -1;
    }
    const long long NS_PER_MIN = 1000000000 * 60LL;
    long long sumNanoS = 0;
    int i = (nextHistEntryIdx - (BPM_BUFFER_SIZE));
    for (int iters = 0; iters < BPM_BUFFER_SIZE - 1; ++iters, ++i)
    {
        //time of i+1's event - time of i's event = time between 2 presses
        sumNanoS += buttonPressHistoryNanoS[(i + 1) % BPM_BUFFER_SIZE] - buttonPressHistoryNanoS[i % BPM_BUFFER_SIZE];
    }
    //ns/min / ns/beat
    bpm = NS_PER_MIN / (sumNanoS / (BPM_BUFFER_SIZE - 1));
    return bpm;
}

long long ButtonHistory_getBPM() 
{
    return bpm;
}

void ButtonHistory_clearHistory() 
{
    memset(buttonPressHistoryNanoS, 0, sizeof(*buttonPressHistoryNanoS) * BPM_BUFFER_SIZE);
    nextHistEntryIdx = 0;
}

void ButtonHistory_initButtonHistory()
{
    nextHistEntryIdx = 0;    
    memset(buttonPressHistoryNanoS, 0, sizeof(*buttonPressHistoryNanoS) * BPM_BUFFER_SIZE);
}
