#ifndef BUTTON_HISTORY_H
#define BUTTON_HISTORY_H
#include "Button.h"

//accurate to ~20ms
void ButtonHistory_recordButtonPress(enum buttons button);
//calculates bpm from current buffer and returns it
//returns -1 if history is empty
long long ButtonHistory_calculateBPM();
long long ButtonHistory_getBPM();
//clear the button press history to re-record user's input
void ButtonHistory_clearHistory();
void ButtonHistory_initButtonHistory();

#endif
