#ifndef BUTTON_HISTORY_H
#define BUTTON_HISTORY_H
#include "button.h"

void recordButtonPress(enum buttons button);
long long calculateBPM();
long long getBPM();
void clearHistory();
void initButtonHistory();

#endif
