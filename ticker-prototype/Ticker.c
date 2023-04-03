#define TICK_FILE "metronome-tick.wav"
#define TICK_SAMPLE_COUNT 700 // How many samples long the tick should be. (reasonable value: 700)

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include "Audio.h"
#include "State.h"

static int16_t *zeros = NULL;
static pthread_t tickerRoutineThread;

static wavedata_t tick;
static wavedata_t silence;
static bool initialized = false;

static void *tickerRoutine(void *args)
{
    while (1)
    {
        int bpm = State_get(ID_BPM);
	if (bpm == 0) 
	{
	    fprintf("BPM cannot be zero\n");
	    exit(1);
	}
	bool isPaused = State_get(ID_ISPAUSED);
	if (isPaused) 
	{
	    silence.numSamples = SAMPLE_RATE; // arbitrary number of samples
	    Audio_play(&silence);
	}
	else 
	{	
            silence.numSamples = SAMPLE_RATE * (60.0 / bpm) - TICK_SAMPLE_COUNT;
            Audio_play(&tick);
            Audio_play(&silence);
	}
    }
    return NULL;
}

// Relies on Audio and State modules being initialized
void Ticker_init()
{   
    int numSamples = Audio_load(TICK_FILE, &tick, TICK_SAMPLE_COUNT);
    if (numSamples != TICK_SAMPLE_COUNT) return; // Requested too many samples

    int maxSamples = SAMPLE_RATE * 60; // At most 60 seconds of silence (1bpm)
    zeros = malloc(SAMPLE_SIZE * maxSamples);
	memset(zeros, 0, SAMPLE_SIZE * maxSamples);
    
    silence.numSamples = maxSamples;
    silence.pData = zeros;

    pthread_create(&tickerRoutineThread, NULL, tickerRoutine, NULL);
    initialized = true;
}

void Ticker_cleanup()
{
    if (initialized) {
        pthread_cancel(tickerRoutineThread);
        pthread_join(tickerRoutineThread, NULL);
        free(tick.pData);
        free(silence.pData);
    }
}
