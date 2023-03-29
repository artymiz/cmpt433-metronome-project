enum buttons {
    BUTTON_ON_OFF = 0,
    //BUTTON_START_STOP_METRONOME,
    //BUTTON_INCREASE_TEMPO, 
    //BUTTON_DECREASE_TEMPO,
    //BUTTON_RECORD_TEMPO,
    NUM_BUTTONS = 1
};
//returns true if button is pressed
int isPressed(enum buttons button);

//returns true if button has been held for the button's holdDelayMs
//note: the duration the button is held for will have a degree of jitter to it, and will not be accurate to an exact delay.
//as the hold delay gets longer, this jitter wil become more apparent. 
//DO NOT USE THIS MODULE IF YOU NEED HIGH ACCURACY TIMING
int isHeld(enum buttons button);

//Initialize multiple buttons that are read when the voltage on a pin sampled is HIGH 
//
//  gpioPinNums: GPIO pin numbers as defined by beaglebone software, *NOT the physical pin number*
//  numButtons:  number of buttons to initialize, must equal NUM_BUTTONS as defined in button.h
void initButtons(int* gpioPinNumbers, int numButtons);
//frees system resources and kills any threads started by button module
void cleanupButtons();
int getHoldDelay(enum buttons button);
void setHoldDelay(enum buttons button, int newHoldDelayMs);
