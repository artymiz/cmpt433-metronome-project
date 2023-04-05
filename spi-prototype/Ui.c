#include "Ui.h"
#include "Graphics.h"
#include "Display.h"
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>

#define RGB_WHITE 0xffffff
#define RGB_BLACK 0x000000
#define RGB_PLAY_OUTLINE  0x013220
#define RGB_PAUSE_OUTLINE RGB_BLACK
#define RGB_BACKGROUND RGB_WHITE
#define RGB_PLAY_FILL 0x00ff00
#define STROKE_TIMESIG 4
#define H_TIMESIG 100

#define PADDING_LAYOUT 10
#define FONTSIZE_LARGE 5
#define FONTSIZE_SMALL 2
#define Y_BPM 130
#define Y_VOL 180
#define X_NUM 140
#define Y_SAMPLE_LBL 145
#define X_SAMPLE_LBL 252
#define Y_SAMPLE_NUM 170
#define X_SAMPLE_NUM 270

#define SLEN_MAX 4


static char *_sbuf;

void UI_setBpm(unsigned bpm)
{
    sprintf(_sbuf, "%u", bpm);
    Graphics_writeStr(_sbuf, FONTSIZE_LARGE, X_NUM, Y_BPM);
}


void UI_setVolume(unsigned vol)
{
    sprintf(_sbuf, "%u", vol);
    Graphics_writeStr(_sbuf, FONTSIZE_LARGE, X_NUM, Y_VOL);
}


void UI_setAudioSample(unsigned sampleId)
{
    sprintf(_sbuf, "%u", sampleId);
    Graphics_writeStr(_sbuf, FONTSIZE_LARGE, X_SAMPLE_NUM, Y_SAMPLE_NUM);
}


typedef struct ScreenPos {
    uint16_t x;
    uint16_t y;
}ScreenPos;

// start_positions of the inner cells
static ScreenPos *_cell_positions = NULL;
static uint16_t _w_frame;
static uint16_t _w_cell;
static const uint16_t _h_cell = H_TIMESIG - 2 * STROKE_TIMESIG;
static int _idx_last_tick;
static unsigned _timesig;

void UI_setTimeSignature(unsigned timeSig)
{
    assert(timeSig > 0);
    _idx_last_tick = -1;
    _timesig = timeSig;

    //clear timesignature screen
     Graphics_fillRect(0, PADDING_LAYOUT,
                       ROW_MAX, H_TIMESIG, RGB_BACKGROUND);

    if (_cell_positions) { free(_cell_positions); }
    _cell_positions = malloc(timeSig * sizeof(ScreenPos));

     _w_frame = (ROW_MAX - 2 * PADDING_LAYOUT) / timeSig + STROKE_TIMESIG;
     _w_cell = _w_frame - 2 * STROKE_TIMESIG;

     uint16_t x = PADDING_LAYOUT;
     for (int i = 0; i < timeSig; i++) {
         Graphics_drawRect(x, PADDING_LAYOUT, _w_frame, H_TIMESIG,
                           STROKE_TIMESIG, RGB_PLAY_OUTLINE);
         _cell_positions[i].x = x + STROKE_TIMESIG;
         _cell_positions[i].y = PADDING_LAYOUT + STROKE_TIMESIG;
         x += _w_frame - STROKE_TIMESIG;
     }
}

static void emptyLastCell(void)
{
    if (_idx_last_tick == -1) { return; }
    Graphics_fillRect(_cell_positions[_idx_last_tick].x, _cell_positions[_idx_last_tick].y,
                      _w_cell, _h_cell, RGB_BACKGROUND);
}

void UI_setNoTick(void)
{
    emptyLastCell();
    _idx_last_tick = -1;
}


void UI_setNextTick(void)
{
    emptyLastCell();
    _idx_last_tick = (_idx_last_tick == _timesig - 1) ? 0 : _idx_last_tick + 1;
    Graphics_fillRect(_cell_positions[_idx_last_tick].x, _cell_positions[_idx_last_tick].y,
                      _w_cell, _h_cell, RGB_PLAY_FILL);
}


void UI_init(unsigned bpm, unsigned vol, unsigned sampleId, unsigned timeSig)
{
    _sbuf = malloc(SLEN_MAX);
    Graphics_init();
    Graphics_writeStr("BPM:", FONTSIZE_LARGE, PADDING_LAYOUT, Y_BPM);
    UI_setBpm(bpm);
    Graphics_writeStr("VOL:", FONTSIZE_LARGE, PADDING_LAYOUT, Y_VOL);
    UI_setVolume(vol);
    Graphics_writeStr("Audio", FONTSIZE_SMALL, X_SAMPLE_LBL, Y_SAMPLE_LBL);
    UI_setAudioSample(sampleId);
    UI_setTimeSignature(timeSig);
}


void UI_cleanUp(void)
{
    free(_sbuf);
    free(_cell_positions);
    Graphics_cleanup();
}

