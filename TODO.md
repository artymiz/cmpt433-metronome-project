Put ticking in a thread and add button-based tempo control

Mutex for silence wavdata_t?

Make ticker a module

Make a module for metronome state, use an enum for getting offset to different parameters
```
#include <stdio.h>

int main(int argc, char const *argv[])
{
    FILE *file = fopen("test.bin", "r+");
    if (file)
    {
        fseek(file, 2, SEEK_SET);
        int a = 2;
        fwrite(&a, sizeof(int), 1, file);
        // a = 3;
        // fwrite(&a, sizeof(int), 1, file);
    }
    fclose(file);
    return 0;
}
```
