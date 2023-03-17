/*
 *  Audio module to read a 16-bit, signed, 44.1kHz wave file and play it.
 *  Written by Brian Fraser, heavily based on code found at:
 *  http://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2pcm_min_8c-example.html
 */

#include <alsa/asoundlib.h>
#include "Audio.h"

static snd_pcm_t *handle;

// Open the PCM audio output device and configure it.
// Returns a handle to the PCM device; needed for other actions.
void Audio_init()
{
	// Open the PCM output
	int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("ERROR: Audio_init: Play-back open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Configure parameters of PCM output
	err = snd_pcm_set_params(handle,
			SND_PCM_FORMAT_S16_LE,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			NUM_CHANNELS,
			SAMPLE_RATE,
			1,			// Allow software resampling
			50000);		// 0.05 seconds per buffer
	if (err < 0) {
		printf("ERROR: Audio_init: Play-back configuration error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
}

// Copy WAV contents into wavedata_t->pData.
// Size of pData array depends on how long the WAV file is.
// !! Client code must free memory in wavedata_t !!
// Returns the number of samples loaded OR the maximum number of samples that can be loaded.
int Audio_load(char *fileName, wavedata_t *pWaveData, int sampleCountRequested)
{
	assert(pWaveData);

	// Open file
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		fprintf(stderr, "ERROR: Audio_load: Unable to open file %s.\n", fileName);
		exit(EXIT_FAILURE);
	}

	// Check that the first 4 bytes are RIFF
	char *riff = "RIFF";
	char riff_test[5];
	fread(riff_test, 1, 4, file);
	riff_test[4] = '\0';
	if (strcmp(riff, riff_test) != 0) {
		fprintf(stderr, "WARNING: Audio_load: %s is not WAV or is corrupted.\n", fileName);
	}

	const int DATA_OFFSET_INTO_WAVE = 44;
	fseek(file, 0, SEEK_END);
	int sampleCountMax = (ftell(file) - DATA_OFFSET_INTO_WAVE) / SAMPLE_SIZE;

	if (sampleCountRequested == -1) {
		// Load them all.
		pWaveData->numSamples = sampleCountMax;
	} else {
		if (sampleCountRequested > sampleCountMax) {
			fprintf(stderr, "WARNING: Could not load %d samples from %s: file too short.\n", sampleCountRequested, fileName);
			fclose(file);
			return sampleCountMax;
		}
		pWaveData->numSamples = sampleCountRequested;
	}

	// Allocate Space
	int numBytes = pWaveData->numSamples * SAMPLE_SIZE;
	pWaveData->pData = malloc(numBytes);
	if (pWaveData->pData == NULL) {
		fprintf(stderr, "ERROR: Audio_load: Unable to allocate %d bytes for file %s.\n", numBytes, fileName);
		exit(EXIT_FAILURE);
	}

	// Read data:
	fseek(file, DATA_OFFSET_INTO_WAVE, SEEK_SET);
	int samplesRead = fread(pWaveData->pData, SAMPLE_SIZE, pWaveData->numSamples, file);
	if (samplesRead != pWaveData->numSamples) {
		fprintf(stderr, "ERROR: Audio_load: Unable to read %d samples from file %s (read %d).\n",
				pWaveData->numSamples, fileName, samplesRead);
		exit(EXIT_FAILURE);
	}

	fclose(file);
	return pWaveData->numSamples;
}

// Play wavedata
void Audio_play(wavedata_t *pWaveData)
{
	// If anything is waiting to be written to screen, can be delayed unless flushed.
	fflush(stdout);

	// Write data and play sound (blocking)
	snd_pcm_sframes_t frames = snd_pcm_writei(handle, pWaveData->pData, pWaveData->numSamples);
	
	// Check for errors
	if (frames < 0)
		frames = snd_pcm_recover(handle, frames, 0);
	if (frames < 0) {
		fprintf(stderr, "ERROR: Audio_play: Failed writing audio with snd_pcm_writei(): %li\n", frames);
		exit(EXIT_FAILURE);
	}
	if (frames > 0 && frames < pWaveData->numSamples)
		printf("WARNING: Audio_play: Short write (expected %d, wrote %li)\n", pWaveData->numSamples, frames);
}

void Audio_cleanup()
{
	// Cleanup, letting the music in buffer play out (drain), then close and free.
	snd_pcm_drain(handle);
	snd_pcm_hw_free(handle);
	snd_pcm_close(handle);
}
