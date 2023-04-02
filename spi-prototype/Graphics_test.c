#include  "Graphics.h"
#include <unistd.h>
#include <stdio.h>

static void characterTest(void)
{
    for (unsigned char i = 32; i < 128; i++) {
        Graphics_writeChar(i, 1, 120, 300);
        Graphics_writeChar(i, 2, 120, 250);
        Graphics_writeChar(i, 3, 120, 210);
        Graphics_writeChar(i, 4, 120, 160);
        Graphics_writeChar(i, 6, 120, 40);
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
        Graphics_writeStr(buf, 1, 230, 10);
        Graphics_writeStr(buf, 3, 200, 10);
        Graphics_writeStr(buf, 4, 100, 10);
        Graphics_writeStr(buf, 5, 0, 10);
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
