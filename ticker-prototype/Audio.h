#include <alsa/asoundlib.h>

#define SAMPLE_RATE   44100
#define NUM_CHANNELS  1
#define SAMPLE_SIZE   (sizeof(short)) 	// bytes per sample

// Store data of a single wave file read into memory.
// Space is dynamically allocated; must be freed correctly!
typedef struct {
	int numSamples;
	short *pData;
} wavedata_t;

// Prototypes:
void Audio_init();
void Audio_load(char *fileName, wavedata_t *pWaveData, int samplesToLoad);
void Audio_play(wavedata_t *pWaveData);
void Audio_cleanup();
