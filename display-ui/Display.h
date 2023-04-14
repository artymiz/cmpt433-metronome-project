#include <stdint.h>
#include <stdlib.h>

#define COL_MAX 240
#define ROW_MAX 320

void Display_init(void);
// buff has the format of RGBRGBRGBRGB.... where R, G, and B are bytes.
void Display_memoryWrite(uint8_t *buff, uint16_t x0, uint16_t y0, uint16_t w, uint16_t h);
void Display_cleanup(void);
