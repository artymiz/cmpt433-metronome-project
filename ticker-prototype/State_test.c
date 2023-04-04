#include <assert.h>
#include <stdio.h>
#include "State.h"

int main(int argc, char const *argv[])
{
    State_load();
    
    printf("%d\n", State_get(ID_BPM));
    printf("%d\n", State_get(ID_TIMESIG));
    printf("%d\n", State_get(ID_BPM));
    printf("%d\n", State_get(ID_VOLUME));

    State_set(ID_TIMESIG, 5);
    State_set(ID_VOLUME, 1000);
    State_set(ID_VOLUME, 44);
    printf("%d\n", State_get(ID_VOLUME));
    printf("%d\n", State_get(ID_TIMESIG));


    State_store();
    
    return 0;
}
