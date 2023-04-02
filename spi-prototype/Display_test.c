#include "Display.h"
#include <unistd.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    Display_init();
    const int edge = 20;
    const int bufLen = edge * edge * 3;
    uint8_t buf[bufLen];
    memset(buf, 0, bufLen);
    Display_memoryWrite(buf, 10, 50, edge, edge);
    Display_cleanup();
    return 0;
}
