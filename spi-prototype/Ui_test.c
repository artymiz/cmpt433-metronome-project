#include "Ui.h"
#include <unistd.h>

void testTimeSig(unsigned timeSig)
{

    UI_setTimeSignature(timeSig);
    sleep(1);

    for (int i = 0; i < 2 * timeSig; i++) {
        UI_setNextTick();
        usleep(300000);
    }

    UI_setNoTick();
    sleep(1);
}

int main(int argc, char *argv[])
{
    UI_init(0, 0, 0, 4);
    testTimeSig(6);
    testTimeSig(16);
    UI_cleanUp();
    return 0;
}

