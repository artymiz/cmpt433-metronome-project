#include <assert.h>
#include <stdio.h>
#include "State.h"

// This test should succeed when state.bin is empty, 
// and SHOULD FAIL afterwards.
int main(int argc, char const *argv[])
{
    State_read();
    
    // Good default values?
    assert(State_getBpm() == 120);
    assert(State_getVolume() == 80);
    assert(State_getBeatsPerBar() == 4);
    
    // Getting and setting while state loaded?
    State_setBpm(70);
    assert(State_getBpm() == 70);

    // Is state saved?
    State_setVolume(50);
    State_write();
    State_read();
    assert(State_getVolume() == 50);
    State_write();

    return 0;
}
