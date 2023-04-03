#ifndef METRONOME_H
#define METRONOME_H

// Checks if the play/pause/changemode/shutdown button was pressed and returns one of:
// METRONOME_PAUSE/METRONOME_PLAY (the REQUESTED state)
// METRONOME_CHANGE_MODE
// METRONOME_KILL_SIGNAL
// METRONOME_NO_CHANGE (button was not pressed)
// playPauseRoutine DOES NOT CHANGE STATE OF METRONOME
int Metronome_playPauseSubroutine();

//checks if volume was changed, 
//checks if tempo was changed, 
//updates state
void Metronome_normalModeSubroutine();

// check if tick button (tempo+) is pressed
// record it and store data
// when enough data is stored, send current bpm to state.
// BPM should be set to 0 at the start of recording, 
// call calculate bpm function
// update metronome state
//         ^metronome displays the rolling total as the bpm is recalculated each button press
// after some amount of time of no input (5s?) set to normal mode at recorded bpm/beats per bar?
// decide as team how this should work before coding.
void Metronome_recordingModeSubroutine();

// Main program loop. Checks and changes the mode of the metronome, and calls the relevant subroutines
void Metronome_mainRoutine();
void Metronome_init();
void Metronome_cleanup();
//changes tempo IFF a button was pressed or held
//the hold duration increases how much tempo is changed by
void Metronome_changeTempo();
//changes tempo IFF a button was pressed or held
//the hold duration increases how much volume is changed by
void Metronome_changeVolume();

#endif