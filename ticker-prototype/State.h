// State can only store non-pointer values!
typedef struct State
{
    int bpm;
    int volume;
    int beatsPerBar;
} State_t;

#define DEFAULT_BPM 120
#define DEFAULT_VOLUME 80
#define DEFAULT_BEATSPERBAR 4

void State_load();
void State_save();

int State_getVolume();
int State_getBpm();
int State_getBeatsPerBar();

void State_setVolume(int newVolume);
void State_setBpm(int newBpm);
void State_setBeatsPerBar(int newBeatsPerBar);
