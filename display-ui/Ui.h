#ifndef _UI_H_
#define _UI_H_

void UI_setBpm(unsigned bpm);
void UI_setVolume(unsigned vol);
void UI_setAudioSample(unsigned sampleId);
void UI_setTimeSignature(unsigned timeSig);
void UI_setNextTick(void);
void UI_setNoTick(void);
void UI_init(unsigned bpm, unsigned vol, unsigned sampleId, unsigned timeSig);
void UI_cleanUp(void);

#endif
