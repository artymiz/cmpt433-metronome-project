#include  "Graphics.h"
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    Graphics_init();

    // Graphics_writeChar('A', 2, 120, 160);
    // Graphics_writeChar('A', 2, 10, 10);
    //Graphics_writeChar('A', 4, 100, 100);
    // char buf[4];
    // for (int i = 0; i < 150; i+=9) {
    //     sprintf(buf, "%d", i);
    //     Graphics_writeStr(buf, 5, 50, 10);
    //     usleep(300000);
    // }
    for (unsigned char i = 32; i < 128; i++) {
        Graphics_writeChar(i, 1, 120, 280);
        Graphics_writeChar(i, 2, 120, 250);
        Graphics_writeChar(i, 3, 120, 210);
        Graphics_writeChar(i, 4, 120, 160);
        Graphics_writeChar(i, 6, 120, 40);
        usleep(350000);
    }
    //Graphics_writeStr("144", 5, 50, 10);
    //Graphics_writeStr("...", 3, 100, 10);
    Graphics_cleanup();
    return 0;
}
