#ifndef BUTTON_H
#define BUTTON_H
#define BUTTON_SAMPLE_RATE_MS 20
enum buttons {
    BUTTON_PLAY_PAUSE_SHUTDOWN = 0,
    //BUTTON_INCREASE_TEMPO, 
    //BUTTON_DECREASE_TEMPO,
    //BUTTON_INCREASE_VOLUME, 
    //BUTTON_DECREASE_VOLUME,
    NUM_BUTTONS
};
//returns true until a button is released
int Button_isPressed(enum buttons button);
//returns true when a button is first pressed, then returns false until the button is released
int Button_justPressed(enum buttons button);

//returns true until the button is released
int Button_isShortHeld(enum buttons button);
//returns true when a button is first held for its shortDelayMs, then returns false until the button is released
int Button_justShortHeld(enum buttons button);

//returns true until the button is released
int Button_isLongHeld(enum buttons button);
//returns true when a button is first held for its longDelayMs, then returns false until the button is released
int Button_justLongHeld(enum buttons button);

//Initialize multiple buttons that are read when the voltage on a pin sampled is HIGH 
//
//  gpioPinNums: GPIO pin numbers as defined by beaglebone software, *NOT the physical pin number*
//  numButtons:  number of buttons to initialize, must equal NUM_BUTTONS as defined in button.h
void Button_initButtons(int* gpioPinNumbers, int numButtons);
//frees system resources and kills any threads started by button module
void Button_cleanupButtons();
int Button_getShortHoldDelay(enum buttons button);
int Button_getLongHoldDelay(enum buttons button);
void Button_setShortHoldDelay(enum buttons button, int newHoldDelayMs);
void Button_setLongHoldDelay(enum buttons button, int newHoldDelayMs);
//returns the time the button has been held in multiples of 20ms
int Button_getTimeHeld(enum buttons button);

#endif