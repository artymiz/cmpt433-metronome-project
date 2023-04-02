#include "Display.h"
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    Display_init();
    const int edge = 20;
    const int bufLen = edge * edge * 3;
    uint8_t buf[bufLen];
    memset(buf, 0, bufLen);
    memset(buf, 0xFF, edge * 3);
    for (int i = 0; i < COL_MAX; i += 20) {
        Display_memoryWrite(buf, i, i, edge, edge);
        sleep(1);
    }
    Display_cleanup();
    return 0;
}
