// File used for play-back:
// If cross-compiling, must have this file available, via this relative path,
// on the target when the application is run. This example's Makefile copies the wave-files/
// folder along with the executable to ensure both are present.
#define SOURCE_FILE "metronome-tick.wav"

#include "Audio-mod/Audio.h"
#include "Timing-mod/Timing.h"
#include <unistd.h>

int main(void)
{
	// Configure Output Device
	Audio_init();

	// Load wave file we want to play:
	wavedata_t tick;
	wavedata_t silence;

	Audio_load(SOURCE_FILE, &tick);

	#define SILENCELEN 43298 // the metronome tick has 802 samples, and one second is 44100 samples

	silence.numSamples = SILENCELEN;
	int16_t zeros[SILENCELEN];
	memset(zeros, 0, SILENCELEN * sizeof(int16_t));
	silence.pData = zeros;

	for (size_t i = 0; i < 10; i++)
	{
		Audio_play(&tick);
		Audio_play(&silence);
		// ! Why does adding sleep cause "ALSA lib pcm.c:8545:(snd_pcm_recover) underrun occurred"
		// ! The buffer needs to be fed after audio init!
		// ! This makes sense, sleep is inaccurate, but the buffer is precise timing wise!
	}


    free(tick.pData);
    Audio_cleanup();

	return 0;
}
