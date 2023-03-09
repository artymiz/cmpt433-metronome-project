#define TICK_FILE "metronome-tick.wav"
#define TICK_SAMPLE_COUNT 700 // How many samples long the tick should be.

#include <assert.h>
#include <pthread.h>
#include "Audio.h"
#include "State.h"

static int16_t *zeros = NULL;
static pthread_t tickerRoutineThread;

static wavedata_t tick;
static wavedata_t silence;

static void *tickerRoutine(void *args)
{
    while (1)
    {
        int bpm = State_getBpm();
        silence.numSamples = SAMPLE_RATE * (60.0 / bpm);
        Audio_play(&tick);
        Audio_play(&silence);
    }
    return NULL;
}

// Relies on Audio and State being initialized
void Ticker_init()
{
    int maxSamples = SAMPLE_RATE * (60.0 / MIN_BPM); // At most 60 seconds of silence (1bpm)
    zeros = malloc(SAMPLE_SIZE * maxSamples);
	memset(zeros, 0, SAMPLE_SIZE * maxSamples);
    
    Audio_load(TICK_FILE, &tick, TICK_SAMPLE_COUNT);
    
    silence.numSamples = maxSamples;
    silence.pData = zeros;

    pthread_create(&tickerRoutineThread, NULL, tickerRoutine, NULL);
}

void Ticker_cleanup()
{
    pthread_cancel(tickerRoutineThread);
    pthread_join(tickerRoutineThread, NULL);
    free(tick.pData);
    free(silence.pData);
}
