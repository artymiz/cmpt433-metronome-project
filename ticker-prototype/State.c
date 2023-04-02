#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "State.h"

static int state[STATECOUNT]; // bpm, volume, beatsPerBar
// exclusive boundaries for state values
static float stateMins[] = {0, -1, 0};
static float stateMaxs[] = {INFINITY, 101, INFINITY};

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
