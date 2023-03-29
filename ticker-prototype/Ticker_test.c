#include "Audio.h"
#include "State.h"
#include "Ticker.h"
#include <unistd.h>
#include <pthread.h>

pthread_t tempoChangerThread;

void *tempoChangerRoutine(void *args)
{
    while (1) {
        char ch = getchar();
        int bpm = State_get(ID_BPM);
        if (ch == 'w') {
            State_set(ID_BPM, bpm + 5);
        } 
        else if (ch == 's') {
            State_set(ID_BPM, bpm - 5);
        }
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    Audio_init();
    State_load();
    Ticker_init();
    pthread_create(&tempoChangerThread, NULL, tempoChangerRoutine, NULL);
    sleep(10);
    pthread_cancel(tempoChangerThread);
    pthread_join(tempoChangerThread, NULL);
    Ticker_cleanup();
    State_store();
    Audio_cleanup();
    return 0;
}
