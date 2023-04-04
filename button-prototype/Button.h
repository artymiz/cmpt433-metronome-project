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
//returns true for at least 20ms, and will remain true (while the button is held) until Button_isShortHeld is true
//the reason ispressed is true until hold is true is because if we had it be true for oinly the first 20ms,
//we would get a weird case where it is not pressed or held, but the time held is still increasing
int Button_isPressed(enum buttons button);

//returns true if button has been held for the button's shortHoldDelayMs
//note: the duration the button is held for will have a degree of jitter to it, and will not be accurate to an exact delay.
//as the hold delay gets longer, this jitter wil become more apparent.
//do not use this module if you need high accuracy timing for button holding
int Button_isShortHeld(enum buttons button);

//returns true if button has been held for the button's longHoldDelayMs
//note: the duration the button is held for will have a degree of jitter to it, and will not be accurate to an exact delay.
//as the hold delay gets longer, this jitter wil become more apparent.
//do not use this module if you need high accuracy timing for button holding
int Button_isLongHeld(enum buttons button);

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