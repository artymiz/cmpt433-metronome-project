// State is described with integers State.txt


typedef enum stateid {
    ID_BPM,
    ID_VOLUME,
    ID_TIMESIG,
    ID_MODE,
    ID_ISPAUSED,
    ID_SAMPLE,
    STATECOUNT
} stateid_t;

//load metronome state from a file
void State_load(void);
//save state to file
void State_store(void);
int State_get(stateid_t id);
//val passed is clamped to a min and max
void State_set(stateid_t id, int val);
