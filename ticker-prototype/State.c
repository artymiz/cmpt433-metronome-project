#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "State.h"

static int state[STATECOUNT]; // bpm, volume, beatsPerBar, mode (normal/recording), isPaused
// exclusive boundaries for state values
static float stateMins[] = {0,  //BPM
                            -1, //volume
                            0,  //beats per bar
                            -1, //mode
                            -1  //is paused
                           };
static float stateMaxs[] = {INFINITY, //BPM
                            101,      //volume
                            INFINITY, //beats per bar
                            2,        //mode
                            2         //is paused
                            };

// File to array
void State_load()
{
    FILE *fState = fopen("State.txt", "r");
    if (!fState) {
        perror("Cannot open State.txt");
        exit(1);
    }
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int id = 0;
    while ((read = getline(&line, &len, fState)) != -1) {
        state[id] = atoi(line);
        id++;
    }
    free(line);
    fclose(fState);
}

// Array to file
void State_store()
{
    FILE *fState = fopen("State.txt", "w");
    if (!fState)
    {
        perror("Cannot open State.txt");
        exit(1);
    }
    for (size_t id = 0; id < STATECOUNT; id++)
    {
        char str[5]; // 3 digits, newline, line terminator
        sprintf(str, "%d\n", state[id]);
        fwrite(str, 1, strlen(str), fState);
    }
    fclose(fState);
}

int State_get(stateid_t id) 
{
    return state[id];
}

void State_set(stateid_t id, int val)
{
    float valFloat = val;
    if (valFloat > stateMins[id] && valFloat < stateMaxs[id])
    {
        state[id] = val;
    }
}
