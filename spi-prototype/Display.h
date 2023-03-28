#include <stdint.h>
#include <stdlib.h>

void Display_init(void);
void Display_memoryWriteArea(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h);
void Display_sendData(uint8_t *buff, size_t bufflen);
