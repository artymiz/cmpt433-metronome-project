// State is described with integers State.txt

#define STATECOUNT 3 // How many integers in state.

typedef enum stateid {
    ID_BPM,
    ID_VOLUME,
    ID_BEATSPERBAR
} stateid_t;

void State_load(void);
void State_store(void);
int State_get(stateid_t id);
void State_set(stateid_t id, int val);
