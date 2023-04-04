#ifndef METRONOME_H
#define METRONOME_H

// Main program loop. Checks and changes the mode of the metronome, 
// and invokes relevant mode funtions
void Metronome_mainThread();
void Metronome_init();
void Metronome_cleanup();
//changes tempo IFF a button was pressed or held
//the hold duration increases how much tempo is changed by
void Metronome_changeTempo();
//changes tempo IFF a button was pressed or held
//the hold duration increases how much volume is changed by
void Metronome_changeVolume();

#endif