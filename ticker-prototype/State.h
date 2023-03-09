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

#define MIN_BPM 1
#define MIN_VOLUME 0
#define MIN_BEATSPERBAR 1

#define MAX_VOLUME 100

void State_read();
void State_write();

int State_getVolume();
int State_getBpm();
int State_getBeatsPerBar();

void State_setVolume(int newVolume);
void State_setBpm(int newBpm);
void State_setBeatsPerBar(int newBeatsPerBar);
