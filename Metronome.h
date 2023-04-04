#ifndef METRONOME_H
#define METRONOME_H
#include <stdbool.h>

//a struct to bundle all of the settings that are needed to change a state value 
struct configCommand {
    //MAY BE ALTERED, the button to check the state of (DEC OR INC buttons only)
    enum buttons button;
    //MAY BE ALTERED, 1 if increasing a setting, -1 if decreasing
    int direction;
    //DO NOT ALTER
    long long lastChangeTimestamp;
    //DO NOT ALTER
    enum stateid stateId;
    //DO NOT ALTER, order is: pressed, shorthold, longhold
    int deltas[3];
}; 
// Main program loop. Checks and changes the mode of the metronome, 
// and invokes relevant mode funtions
void Metronome_mainThread();
void Metronome_init();
void Metronome_cleanup();
//changes tempo IFF a button was pressed or held
//the hold duration increases how much tempo is changed byvoid 
void Metronome_changeStateSetting(struct configCommand* command);



#endif