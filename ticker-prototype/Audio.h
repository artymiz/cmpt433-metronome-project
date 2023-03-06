#include <alsa/asoundlib.h>

// Store data of a single wave file read into memory.
// Space is dynamically allocated; must be freed correctly!
typedef struct {
	int numSamples;
	short *pData;
} wavedata_t;

// Prototypes:
void Audio_init();
void Audio_load(char *fileName, wavedata_t *pWaveStruct);
void Audio_play(wavedata_t *pWaveData);
void Audio_cleanup();
