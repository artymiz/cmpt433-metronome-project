#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Display.h"
#include "font5x7.h"
#include "Graphics.h"
#include <stdio.h>

#define MAX_FONT_SIZE 8
#define RGB_LEN 3
#define BLACK 0x000000
#define WHITE 0xffffff

static uint32_t _txt_rgb;


// =============================
// Configuration/ Init / Cleanup
// ============================

void Graphics_init(void)
{
    Display_init();
    Graphics_setTextColor(BLACK);
}

void Graphics_cleanup(void)
{
    Display_cleanup();
}


void Graphics_setTextColor(uint32_t rgb)
{
    _txt_rgb = rgb;
}

// =====================
// Misc Helper Functions
// =====================

static void setPixel(uint8_t *buff, uint8_t bit)
{
    if (bit == 1) {
        memcpy(buff, (char*)&_txt_rgb, RGB_LEN);
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
            // if (bit == 1) printf("\t%d\n", diff);
            setPixel(buff + offset + (i * w + j), bit);
        }
    }
}


// ===========================
// Displaying Single Character
// ===========================


// write a character at position x0 and y0 from the top right corner of the screen
static void writeChar(unsigned char c, uint8_t fontsize, uint16_t x0, uint16_t y0,
        uint8_t leadingSpace)
{
    assert(fontsize > 0 && fontsize <= MAX_FONT_SIZE);
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
        uint16_t n = leadingSpace * h * fontsize * RGB_LEN;
        uint16_t offset = h * w * RGB_LEN - n;
        memset(buff + offset, 0xff, n);
    }

    Display_memoryWrite(buff, x0, y0, h, w);
    free(buff);
}


// translates the y position from the top of the screen in landscape
// to the actual x Display position where the drawing starts
static uint16_t getDisplayX(uint16_t y0, uint16_t drawingH)
{
    return COL_MAX - (y0 + drawingH);
}


// translates the x position from the left of the screen in landscape
// to the actual y Display position where the drawing starts
static uint16_t getDisplayY(uint16_t x0, uint16_t drawingW)
{
    return ROW_MAX - (x0 + drawingW);
}


void Graphics_writeChar(unsigned char c, uint8_t fontsize, uint16_t x0, uint16_t y0)
{
    uint16_t cWidth = fontsize * FONT5X7_WIDTH;
    uint16_t cHeight = fontsize * FONT5X7_HEIGHT;
    writeChar(c, fontsize, getDisplayX(y0, cHeight), getDisplayY(x0, cWidth), 0);
}


// ================
// Display Strings
// ================


static uint8_t _str_spread = 2;

// sets how far apart the characters are relative to each other.
// Scales with the font size - #pixel apart = font size * spread
void Graphics_setStrSpread(uint8_t spread)
{
    assert(spread <= MAX_FONT_SIZE);
    _str_spread = spread;
}


// write a string from the top right corner of the screen in vertical position,
// in the character's upright orientation,
// the corner specified by x0 and y0 will appear to be bottom right
static void writeStr(char *s, uint8_t fontsize, uint16_t x0, uint16_t y0)
{
    size_t n = strlen(s);
    uint16_t cWidth = (FONT5X7_WIDTH + _str_spread) * fontsize;
    uint16_t y = y0;

    for (int i = n - 1; i > 0; i--, y += cWidth) {
        writeChar(s[i], fontsize, x0, y, _str_spread);
    }
    writeChar(s[0], fontsize, x0, y, 0);
}


void Graphics_writeStr(char *s, uint8_t fontsize, uint16_t x0, uint16_t y0)
{
    size_t n = strlen(s);
    uint16_t sWidth = fontsize * (FONT5X7_WIDTH * n + _str_spread * (n - 1));
    uint16_t sHeight = fontsize * FONT5X7_HEIGHT;
    writeStr(s, fontsize, getDisplayX(y0, sHeight), getDisplayY(x0, sWidth));
}


// ===================
// Rectangle Drawing
// ===================

static int _rect_stroke;

static void setFilledRectBuff(uint8_t *buff, uint16_t w, uint16_t h, uint32_t rgb)
{
    uint n = w * h * RGB_LEN;
    for (int i = 0; i < n; i += RGB_LEN) {
        memcpy(buff + i, (char*)&rgb, RGB_LEN);
    }
}

static void setEmptyRectBuff(uint8_t *buff, uint16_t w, uint16_t h, uint32_t rgb)
{
}

typedef void (*setBuffFuncPtr_t)(uint8_t*, uint16_t, uint16_t, uint32_t);

static void writeRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
                      uint32_t rgb, setBuffFuncPtr_t fp)
{
    uint n = w * h * RGB_LEN;
    uint8_t *buff = malloc(n);
    fp(buff, w, h, rgb);
    Display_memoryWrite(buff, getDisplayX(y0, h), getDisplayY(x0, w), h, w);
    free(buff);
}

// Draw an outline of a rectangle
void Graphics_drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
                       int strokeSize, uint32_t rgb)
{
    _rect_stroke = strokeSize;
    writeRect(x0, y0, w, h, rgb, setEmptyRectBuff);
}


// Draw a filled rectangle
void Graphics_fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h,
        uint32_t rgb)
{
    writeRect(x0, y0, w, h, rgb, setFilledRectBuff);
}


