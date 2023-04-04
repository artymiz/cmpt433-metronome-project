#include "Graphics.h"
#include "Display.h"
#include <unistd.h>
#include <stdio.h>

static void characterTest(void)
{
    for (unsigned char i = 32; i < 128; i++) {
        Graphics_writeChar(i, 1, 120, 220);
        Graphics_writeChar(i, 2, 120, 190);
        Graphics_writeChar(i, 3, 120, 150);
        Graphics_writeChar(i, 6, 120, 30);
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

static void filledRectTest(__useconds_t usecs, uint32_t rgb)
{
     usleep(usecs);
     uint16_t w = 50;
     uint16_t h = 100;
     uint16_t padding = 10;
     uint16_t x_end = ROW_MAX - w - padding;
     for (int x = padding; x < x_end; x += (w + padding)) {
         Graphics_fillRect(x, padding, w, h, rgb);
         usleep(usecs);
         // redraw with white
         Graphics_fillRect(x, padding, w, h, 0xffffff);
     }
}

int main(int argc, char *argv[])
{
    Graphics_init();
    // Graphics_setTextColor(0xff0000);
    // characterTest();
    // strTest();
    uint32_t color = 0xff0000;
    filledRectTest(500000, color);
    Graphics_drawRect(10, 10, 40, 80, 4, color);
    Graphics_drawRect(50, 10, 40, 80, 4, color);
    Graphics_cleanup();
    return 0;
}
