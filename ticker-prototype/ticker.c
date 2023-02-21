// File used for play-back:
// If cross-compiling, must have this file available, via this relative path,
// on the target when the application is run. This example's Makefile copies the wave-files/
// folder along with the executable to ensure both are present.
#define SOURCE_FILE "metronome-tick.wav"

#include "Audio-mod/Audio.h"

int main(void)
{
	printf("Beginning play-back of %s\n", SOURCE_FILE);

	// Configure Output Device
	Audio_init();

	// Load wave file we want to play:
	wavedata_t wavedata;
	Audio_load(SOURCE_FILE, &wavedata);

	// Play Audio
	Audio_play(&wavedata);
//	Audio_playFile(handle, &wavedata);
//	Audio_playFile(handle, &wavedata);

    free(wavedata.pData);
    Audio_cleanup();

	// printf("Done!\n");
	return 0;
}
