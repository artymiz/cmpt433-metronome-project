// File used for play-back:
// If cross-compiling, must have this file available, via this relative path,
// on the target when the application is run. This example's Makefile copies the wave-files/
// folder along with the executable to ensure both are present.
#define TICK_FILE "metronome-tick.wav"
#define TICK_SAMPLE_COUNT 700 // How many samples long the tick should be.

#include "Audio.h"
#include <assert.h>

// Set pWaveData->pData to point to an array of zeros, number of zeros depends on bpm.
void silence_load(int bpm, wavedata_t *pWaveData, int tickSampleCount)
{
	float beatDurationSeconds = 60.0 / bpm;
	int silenceSampleCount = SAMPLE_RATE * beatDurationSeconds - tickSampleCount; // Together, silence and the tick take up a beat.
	int16_t *zeros = malloc(SAMPLE_SIZE * silenceSampleCount);
	memset(zeros, 0, silenceSampleCount * SAMPLE_SIZE);
	pWaveData->numSamples = silenceSampleCount;
	pWaveData->pData = zeros;
}

int main(int argc, char const *argv[])
{
	assert(argc == 2);
	int bpm = atoi(argv[1]);

	// Configure Output Device.
	Audio_init();

	// Load wave file and silence we want to play.
	wavedata_t tick;
	wavedata_t silence;
	Audio_load(TICK_FILE, &tick, TICK_SAMPLE_COUNT);
	silence_load(bpm, &silence, TICK_SAMPLE_COUNT);
	
	for (size_t i = 0; i < 10; i++)
	{
		Audio_play(&tick);
		Audio_play(&silence);
		// sleep(1);
		// ! Why does adding sleep cause "ALSA lib pcm.c:8545:(snd_pcm_recover) underrun occurred"
		// ! The buffer needs to be fed after audio init!
		// ! This makes sense, sleep is inaccurate, but the buffer is precise timing wise!
	}

    free(tick.pData);
	free(silence.pData);
    Audio_cleanup();

	return 0;
}
