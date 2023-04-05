#include <stddef.h>
#include <stdint.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

void Graphics_init(void);
void Graphics_setTextColor(uint32_t rgb);
void Graphics_writeChar(unsigned char c, uint8_t fontsize, uint16_t x0, uint16_t y0);
void Graphics_setStrSpread(uint8_t spread);
void Graphics_writeStr(char *s, uint8_t fontsize, uint16_t x0, uint16_t y0);
void Graphics_drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
                       int strokeSize, uint32_t rgb);
void Graphics_fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint32_t rgb);
void Graphics_cleanup(void);

#endif
