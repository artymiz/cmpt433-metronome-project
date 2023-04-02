#ifndef BUTTON_H
#define BUTTON_H
enum buttons {
    BUTTON_PLAY_PAUSE_SHUTDOWN = 0,
    //BUTTON_START_STOP_METRONOME,
    //BUTTON_INCREASE_TEMPO, 
    //BUTTON_DECREASE_TEMPO,
    //BUTTON_RECORD_TEMPO,
    NUM_BUTTONS = 1
};
//returns true for at least 20ms, and will remain true (while the button is held) until isShortHeld is true
int isPressed(enum buttons button);

//returns true if button has been held for the button's shortHoldDelayMs
//note: the duration the button is held for will have a degree of jitter to it, and will not be accurate to an exact delay.
//as the hold delay gets longer, this jitter wil become more apparent.
//do not use this module if you need high accuracy timing for button holding
int isShortHeld(enum buttons button);
//returns true if button has been held for the button's longHoldDelayMs
//note: the duration the button is held for will have a degree of jitter to it, and will not be accurate to an exact delay.
//as the hold delay gets longer, this jitter wil become more apparent.
//do not use this module if you need high accuracy timing for button holding
int isLongHeld(enum buttons button);

//Initialize multiple buttons that are read when the voltage on a pin sampled is HIGH 
//
//  gpioPinNums: GPIO pin numbers as defined by beaglebone software, *NOT the physical pin number*
//  numButtons:  number of buttons to initialize, must equal NUM_BUTTONS as defined in button.h
void initButtons(int* gpioPinNumbers, int numButtons);
//frees system resources and kills any threads started by button module
void cleanupButtons();
int getShortHoldDelay(enum buttons button);
int getLongHoldDelay(enum buttons button);
void setShortHoldDelay(enum buttons button, int newHoldDelayMs);
void setLongHoldDelay(enum buttons button, int newHoldDelayMs);
//returns the time the button has been held in multiples of 20ms
int getTimeHeld(enum buttons button);

#endif