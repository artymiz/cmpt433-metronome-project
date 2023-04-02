#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Display.h"
#include "font5x7.h"
#include "Graphics.h"

#define MAX_FONT_SIZE 10
#define RGB_LEN 3
#define BLACK 0x000000
#define WHITE 0xffffff

static uint32_t _bgr;

void Graphics_init(void)
{
    Display_init();
    Graphics_setColor(BLACK);
}

void Graphics_setColor(uint32_t bgr)
{
    _bgr = bgr;
}

static void setPixel(uint8_t *buff, uint8_t bit)
{
    // when copied the buffer will receive in reverse order
    // ie. 0x123456 -> buff[0] = 0x56, buff[1] = 0x34, buff[2] = 0x12
    if (bit) {
        memcpy(buff, (unsigned char*)&_bgr, RGB_LEN);
    } else {
        // set to white background color
        memset(buff, 0xff, RGB_LEN);
    }
}

static void setBlock(uint8_t *buff, uint8_t bit, uint16_t w, uint16_t blocksize)
{
    const int n = blocksize * RGB_LEN;
    for (int i = 0; i < n; i+=RGB_LEN) {
        for (int j = 0; j < n; j+=RGB_LEN) {
            setPixel(buff + i * w + j, bit);
        }
    }
}

// write a character at position x0 and y0 from the top right corner of the screen
static void writeChar(unsigned char c, uint8_t fontsize, uint16_t x0, uint16_t y0,
        uint8_t trailingSpace)
{
    assert(fontsize > 0 && fontsize < MAX_FONT_SIZE);
    const int h = FONT5X7_HEIGHT * fontsize;
    const int w = (FONT5X7_WIDTH + trailingSpace) * fontsize;

    uint8_t buff[h * w * RGB_LEN];

    // set white space at the right side of the character
    if (trailingSpace > 0) {
        memset(buff, 0xff, trailingSpace * h * RGB_LEN);
    }

    for (int row = trailingSpace; row < FONT5X7_WIDTH; row++) {
        // read the values of the character in reverse order
        unsigned char cRowVal = font5x7[(c + 1) * FONT5X7_WIDTH - row - 1];

        for (int col = 0; col < FONT5X7_HEIGHT; col++) {
            unsigned char shift = FONT5X7_HEIGHT - col - 1;
            unsigned char bit = (cRowVal & (1 << shift)) >> shift;
            uint16_t cornerIdx = RGB_LEN * (h * row + col * fontsize);
            setBlock(buff + cornerIdx, bit, h, fontsize);
        }

    }

    Display_memoryWrite(buff, x0, y0, h, w);
}

void Graphics_writeChar(unsigned char c, uint8_t fontsize, uint16_t x0, uint16_t y0)
{
    writeChar(c, fontsize, x0, y0, 0);
}

static const uint8_t _char_sep_space = 1;


// write a string from the top right corner,
// in the character's upright orientation, the corner will appear to be bottom right
void Graphics_writeStr(char *s, uint8_t fontsize, uint16_t x0, uint16_t y0)
{
    size_t len = strlen(s);

    uint16_t y = y0 + fontsize * len *(_char_sep_space + FONT5X7_WIDTH);
    // last character does not have trailing whitespace so subtract by space
    y -= fontsize * _char_sep_space;
    assert(y <= ROW_MAX);

    uint16_t cWidth = (FONT5X7_WIDTH + _char_sep_space) * fontsize;

    for (int i = 0; i < len - 1; i++, y -= cWidth) {
        writeChar(s[i], fontsize, x0, y, _char_sep_space);
    }
    writeChar(s[len - 1], fontsize, x0, y0, 0);
}

void Graphics_writeCenterStr(char *s, uint8_t fontsize, uint16_t y0)
{
}

void Graphics_cleanup(void)
{
    Display_cleanup();
}
