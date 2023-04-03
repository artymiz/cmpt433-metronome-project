// State is described with integers State.txt


typedef enum stateid {
    ID_BPM,
    ID_VOLUME,
    ID_BEATSPERBAR,
    ID_MODE,
    ID_ISPAUSED,
    STATECOUNT
} stateid_t;

void State_load(void);
void State_store(void);
int State_get(stateid_t id);
void State_set(stateid_t id, int val);
