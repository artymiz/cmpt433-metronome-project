#include "Display.h"
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

void displayRectTest(int x0, int y0, int w, int h)
{
    int n = w * h * 3;
    uint8_t *buff = malloc(n);
    for (int i = 0; i < n; i += 3) {
        buff[i] = 0xff;
        buff[i+1] = 0;
        buff[i+2] = 0;
    }
    Display_memoryWrite(buff, x0, y0, w, h);
    free(buff);
}

int main(int argc, char const *argv[])
{
    Display_init();
    // const int edge = 20;
    // const int bufLen = edge * edge * 3;
    // uint8_t buf[bufLen];
    // memset(buf, 0, bufLen);
    // memset(buf, 0xFF, edge * 3);
    // for (int i = 0; i < COL_MAX; i += 20) {
    //     Display_memoryWrite(buf, i, i, edge, edge);
    //     sleep(1);
    // }
    displayRectTest(0, 0, 50, 50);
    displayRectTest(0, 80, 80, 100);
    Display_cleanup();
    return 0;
}
