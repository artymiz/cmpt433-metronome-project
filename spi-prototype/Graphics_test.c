#include  "Graphics.h"

int main(int argc, char *argv[])
{
    Graphics_init();
    Graphics_writeChar('A', 1, 0, 0);
    Graphics_writeStr("ABCDEFGHIJKLMNOP", 1, 0, 0);
    // Graphics_writeChar('A', 2, 10, 10);
    // Graphics_writeChar('A', 3, 50, 50);
    // Graphics_writeChar('A', 4, 100, 100);
    Graphics_cleanup();
    return 0;
}
