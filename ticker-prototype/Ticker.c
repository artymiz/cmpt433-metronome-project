#define TICK_SAMPLE_COUNT 700 // How many samples long the tick should be. (reasonable value: 700)

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include "Audio.h"
#include "State.h"
#include "Ticker.h"

static int16_t *zeros = NULL;
static pthread_t tickerRoutineThread;

static wavedata_t ticks[SAMPLENUM_MAX];
static wavedata_t silence;
static bool initialized = false;

static int circularAdd(int val, int valMax)
{
    if (val < valMax)
    {
        return val + 1;
    }
    return 1;
}

static void *tickerRoutine(void *args)
{
    int currentBeat = 1;
    while (1)
    {
        int bpm = State_get(ID_BPM);
        int timeSig = State_get(ID_TIMESIG);
        int sampleNum = State_get(ID_SAMPLE);

        int volumeBeat1 = State_get(ID_VOLUME);
        int volumeBeatOther = volumeBeat1 * 0.65;

        if (currentBeat == 1)
            Audio_setVolume(volumeBeat1);
        else
            Audio_setVolume(volumeBeatOther);

        if (bpm == 0)
        {
            printf("BPM cannot be zero\n");
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
            Audio_play(&ticks[sampleNum]);
            Audio_play(&silence);
            currentBeat = circularAdd(currentBeat, timeSig);
            // printf("--> Current beat: %d\n", currentBeat);
        }
    }
    return NULL;
}

// Relies on Audio and State modules being initialized
void Ticker_init()
{
    // ticks/0.wav
    char tickString[] = {'t', 'i', 'c', 'k', 's', '/', '0', '.', 'w', 'a', 'v', '\0'};
    for (size_t i = 0; i < SAMPLENUM_MAX; i++)
    {
        tickString[6] = '0' + i;
        int numSamples = Audio_load(tickString, &ticks[i], TICK_SAMPLE_COUNT);
        if (numSamples != TICK_SAMPLE_COUNT)
            return; // Requested too many samples
    }

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
    if (initialized)
    {
        pthread_cancel(tickerRoutineThread);
        pthread_join(tickerRoutineThread, NULL);
        for (size_t i = 0; i < SAMPLENUM_MAX; i++)
        {
            free(ticks[i].pData);
        }
        free(silence.pData);
    }
}
