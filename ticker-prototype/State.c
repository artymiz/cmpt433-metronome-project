#include <stdio.h>
#include <stdbool.h>
#include "State.h"

static State_t state = {
    .bpm = DEFAULT_BPM, 
    .volume = DEFAULT_VOLUME, 
    .beatsPerBar = DEFAULT_BEATSPERBAR
};

static bool emptyFile(FILE *f)
{
    fseek(f, 0, SEEK_END);
    bool isEmpty = (ftell(f) == 0);
    fseek(f, 0, SEEK_SET);
    return isEmpty;
}

void State_read()
{
    FILE *fState = fopen("state.bin", "r");
    if (!emptyFile(fState)) 
    {
        fread(&state, sizeof(State_t), 1, fState);
    }
    fclose(fState);
}

void State_write()
{
    FILE *fState = fopen("state.bin", "w");
    fwrite(&state, sizeof(State_t), 1, fState);
    fclose(fState);
}

int State_getVolume() {return state.volume;}
int State_getBpm() {return state.bpm;}
int State_getBeatsPerBar() {return state.beatsPerBar;}

void State_setVolume(int newVolume) {state.volume = newVolume;}
void State_setBpm(int newBpm) {state.bpm = newBpm;}
void State_setBeatsPerBar(int newBeatsPerBar) {state.beatsPerBar = newBeatsPerBar;}
