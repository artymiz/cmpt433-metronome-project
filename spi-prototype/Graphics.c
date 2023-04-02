#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Display.h"
#include "font5x7.h"
#include "Graphics.h"
#include <stdio.h>

#define MAX_FONT_SIZE 10
#define RGB_LEN 3
#define BLACK 0x000000
#define WHITE 0xffffff

static uint8_t *_rgb;

void Graphics_init(void)
{
    Display_init();
    _rgb = malloc(RGB_LEN);
    Graphics_setColor(0, 0, 0);
}

void Graphics_setColor(uint8_t r, uint8_t g, uint8_t b)
{
    _rgb[0] = r;
    _rgb[1] = g;
    _rgb[2] = b;
}

static void setPixel(uint8_t *buff, uint8_t bit)
{
    if (bit == 1) {
        buff[0] = _rgb[0];
        buff[1] = _rgb[1];
        buff[2] = _rgb[2];
    } else {
        // set to white background color
        memset(buff, 0xff, RGB_LEN);
    }
}

static void setBlock(uint8_t *buff, uint16_t offset, uint8_t bit, uint16_t w, uint16_t blocksize)
{
    const uint n = blocksize * RGB_LEN;
    for (int i = 0; i < n; i+=RGB_LEN) {
        for (int j = 0; j < n; j+=RGB_LEN) {
            int diff = offset + (i* w + j);
            // if (bit == 1) printf("\t%d\n", diff);
            setPixel(buff + offset + (i * w + j), bit);
        }
    }
}

// write a character at position x0 and y0 from the top right corner of the screen
static void writeChar(unsigned char c, uint8_t fontsize, uint16_t x0, uint16_t y0,
        uint8_t leadingSpace)
{
    assert(fontsize > 0 && fontsize < MAX_FONT_SIZE);
    const int h = FONT5X7_HEIGHT * fontsize;
    const int w = (FONT5X7_WIDTH + leadingSpace) * fontsize;

    uint8_t *buff = malloc(h * w * RGB_LEN);

    for (int row = 0; row < FONT5X7_WIDTH; row++) {
        // read the values of the character in reverse order
        unsigned char cRowVal = font5x7[(c + 1) * FONT5X7_WIDTH - row - 1];

        for (int col = 0; col < FONT5X7_HEIGHT; col++) {
            unsigned char shift = FONT5X7_HEIGHT - col - 1;
            unsigned char bit = (cRowVal & (1 << shift)) >> shift;
            uint16_t cornerIdx = RGB_LEN * fontsize * (h * row + col);
            // if (bit == 1) printf("CornerIdx: %u\n", cornerIdx);
            setBlock(buff, cornerIdx, bit, h, fontsize);
        }
    }

    // set white space at the left side of the character
    if (leadingSpace > 0) {
        uint16_t n = leadingSpace * h * RGB_LEN;
        uint16_t offset = h * w * RGB_LEN - n;
        memset(buff + offset, 0xff, n);
    }

    Display_memoryWrite(buff, x0, y0, h, w);
    free(buff);
}

void Graphics_writeChar(unsigned char c, uint8_t fontsize, uint16_t x0, uint16_t y0)
{
    writeChar(c, fontsize, x0, y0, 0);
}

static const uint8_t _char_sep_space = 2;


// write a string from the top right corner,
// in the character's upright orientation, the corner will appear to be bottom right
void Graphics_writeStr(char *s, uint8_t fontsize, uint16_t x0, uint16_t y0)
{
    size_t len = strlen(s);
    uint16_t cWidth = (FONT5X7_WIDTH + _char_sep_space) * fontsize;
    uint16_t y = y0;

    for (int i = len - 1; i > 0; i--, y += cWidth) {
        writeChar(s[i], fontsize, x0, y, _char_sep_space);
    }
    writeChar(s[0], fontsize, x0, y, 0);
}

void Graphics_writeCenterStr(char *s, uint8_t fontsize, uint16_t y0)
{
}

void Graphics_cleanup(void)
{
    free(_rgb);
    Display_cleanup();
}
