#include "Audio.h"
#include "State.h"
#include "Ticker.h"
#include <unistd.h>
#include <pthread.h>

pthread_t tempoChangerThread;

void *tempoChangerRoutine(void *args)
{
    while (1)
    {
        char ch = getchar();
        if (ch == 'w')
            State_setBpm(State_getBpm() + 5);
        else if (ch == 's')
            State_setBpm(State_getBpm() - 5);
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    Audio_init();
    State_read();
    Ticker_init();
    pthread_create(&tempoChangerThread, NULL, tempoChangerRoutine, NULL);
    sleep(10);
    pthread_cancel(tempoChangerThread);
    pthread_join(tempoChangerThread, NULL);
    Ticker_cleanup();
    State_write();
    Audio_cleanup();
    return 0;
}
