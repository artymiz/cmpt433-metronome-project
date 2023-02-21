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
	wavedata_t wavedata;

	Audio_load(SOURCE_FILE, &wavedata);
	
	// int iterTime = 1000; // Time that for loop should take
	for (size_t i = 0; i < 10; i++)
	{
		// int start = Timing_stampMs();
		Audio_play(&wavedata);
		// int end = Timing_stampMs();
		// printf("%d\n", end); // The numbers should be 1000 apart...
		// Timing_waitMs(iterTime - (end - start));
		sleep(1); // ! Why does adding sleep cause "ALSA lib pcm.c:8545:(snd_pcm_recover) underrun occurred"
	}

    free(wavedata.pData);
    Audio_cleanup();

	// printf("Done!\n");
	return 0;
}
