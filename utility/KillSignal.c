#include "KillSignal.h"
#include <pthread.h>

pthread_mutex_t kill_mutex;
static int running = 1;

//send a signal to all other threads to shutdown the program
void KillSignal_shutdown() {
    //printf("killing?");
    pthread_mutex_lock(&kill_mutex);
    running = 0;
    pthread_mutex_unlock(&kill_mutex);
}

//check if KillSignal_shutdown has been called, if it has, the entire program shuts down
int KillSignal_getIsRunning() {
    return running;
}