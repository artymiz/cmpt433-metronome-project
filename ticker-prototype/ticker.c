// File used for play-back:
// If cross-compiling, must have this file available, via this relative path,
// on the target when the application is run. This example's Makefile copies the wave-files/
// folder along with the executable to ensure both are present.
#define SOURCE_FILE "metronome-tick.wav"
#define SOURCE_FILE_SAMPLE_COUNT 802 // how many samples the source file has

#include "Audio.h"
#include <assert.h>

void silence_load(int bpm, wavedata_t *pWaveData)
{
	float beat_length_seconds = 60.0 / bpm;
	int silence_sample_count = SAMPLE_RATE * beat_length_seconds - SOURCE_FILE_SAMPLE_COUNT; // together, silence and the tick take up a beat
	int16_t *zeros = malloc(SAMPLE_SIZE * silence_sample_count);
	memset(zeros, 0, silence_sample_count * SAMPLE_SIZE);
	pWaveData->numSamples = silence_sample_count;
	pWaveData->pData = zeros;
}

int main(int argc, char const *argv[])
{
	assert(argc == 2);
	int bpm = atoi(argv[1]);

	// Configure Output Device
	Audio_init();

	// Load wave file we want to play:
	wavedata_t tick;
	wavedata_t silence;

	Audio_load(SOURCE_FILE, &tick);
	silence_load(bpm, &silence);
	
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
    Audio_cleanup();

	return 0;
}
