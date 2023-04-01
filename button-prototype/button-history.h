#ifndef BUTTON_HISTORY_H
#define BUTTON_HISTORY_H
#include "button.h"

void recordButtonPress(enum buttons button);
int calculateBPM();
int getBPM();
void clearHistory();
void initButtonHistory();

#endif
