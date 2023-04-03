#include  "Graphics.h"
#include <unistd.h>
#include <stdio.h>

static void characterTest(void)
{
    for (unsigned char i = 32; i < 128; i++) {
        Graphics_writeChar(i, 1, 120, 220);
        Graphics_writeChar(i, 2, 120, 190);
        Graphics_writeChar(i, 3, 120, 150);
        Graphics_writeChar(i, 4, 120, 90);
        usleep(250000);
    }
}

static void strTest(void)
{
    char buf[9];
    buf[8] = 0;
    for (unsigned char i = 32; i < 128; i += 8) {
        for (unsigned char j = 0; j < 8; j++) {
            buf[j] = i + j;
        }
        Graphics_writeStr(buf, 1, 10, 20);
        Graphics_writeStr(buf, 3, 10, 30);
        Graphics_writeStr(buf, 4, 10, 50);
        usleep(500000);
    }
}

int main(int argc, char *argv[])
{
    Graphics_init();
    // characterTest();
    strTest();
    Graphics_cleanup();
    return 0;
}
