#ifndef METRONOME_H
#define METRONOME_H
#include <stdbool.h>
// Main program loop. Checks and changes the mode of the metronome, 
// and invokes relevant mode funtions
void Metronome_mainThread();
void Metronome_init();
void Metronome_cleanup();
//changes tempo IFF a button was pressed or held
//the hold duration increases how much tempo is changed byvoid 
void Metronome_changeVolumeBpmOrTimeSignature(enum buttons button, int deltas[3], enum stateid stateId, bool increase);



#endif