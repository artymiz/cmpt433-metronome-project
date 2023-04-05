#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "State.h"
#include <pthread.h>

//mutex for writing/reading state file
pthread_mutex_t stateMutex;
//mutex for altering a single state setting
pthread_mutex_t stateSettingMutex[STATECOUNT];

static int state[STATECOUNT]; // bpm, volume, time signature, mode (normal/recording), isPaused
// inclusive boundaries for state values
static float stateMins[] = {1, //BPM
                            0, //volume
                            1, //time signature
                            0, //mode
                            0  //is paused
                           };
static float stateMaxs[] = {350, //BPM
                            100, //volume
                            100, //time signature
                            1,   //mode
                            1    //is paused
                            };

// File to array
void State_load()
{
    pthread_mutex_lock(&stateMutex);
    FILE *fState = fopen("State.txt", "r");
    if (!fState) {
        perror("Cannot open State.txt");
        pthread_mutex_unlock(&stateMutex);
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
    pthread_mutex_unlock(&stateMutex);
    free(line);
    fclose(fState);
}

// Array to file
void State_store()
{
    pthread_mutex_lock(&stateMutex);
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
    pthread_mutex_unlock(&stateMutex);
    fclose(fState);
}

int State_get(stateid_t id) 
{
    return state[id];
}

void State_set(stateid_t id, int val)
{
    pthread_mutex_lock(&stateSettingMutex[id]);
    if (val < stateMins[id])
    {
        state[id] = stateMins[id];
    }
    else if (val > stateMaxs[id])
    {
        state[id] = stateMaxs[id];
    }
    else 
    {
        state[id] = val;
    }
    pthread_mutex_unlock(&stateSettingMutex[id]);

}
