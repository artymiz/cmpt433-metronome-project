#include "Button.h"
#include "../utility/GPIO.h"
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../utility/Timing.h"
#include "../utility/KillSignal.h"
#define DEFAULT_SHORT_HOLD_DELAY_MS 200
#define DEFAULT_LONG_HOLD_DELAY_MS 500

static void* runSampleLoop();

//for checking if the program is still running (kill switch)
int isRunning = 1;

//a single button on the beaglebone's breadboard
//each button has its own hold delay so that we can have buttons with different delays.
//
//As an example, we can have a reduce tempo button with a short hold delay to rapidly reduce tempo,
//but have a play/pause button with a long hold delay that, when held, will kill the program 
struct button_t {
    int gpioPinNum;
    //how long the button needs to be held until it is considered held down for a short time
    int shortHoldDelayMs;
    //how long the button needs to be held until it is considered held down for a long time
    int longHoldDelayMs;
    //this will be true for up to BUTTON_SAMPLE_RATE_MS + jitter
    int isPressed;
    //this will be true for as long as the button is being sampled as true for longer than shortHoldDelayMs
    int timeHeldMs;
};
static struct button_t buttons[NUM_BUTTONS];
static pthread_t readLoop;

//Initialize multiple buttons that are read when the voltage sampled is HIGH 
//
//gpioPinNums: GPIO pin numbers as defined by beaglebone software, *NOT the physical pin number*
//numButtons:  number of buttons to initialize, must equal NUM_BUTTONS as defined in button.h
void Button_initButtons(int* gpioPinNumbers, int numButtons)
{
    assert(numButtons == NUM_BUTTONS);
    printf("Initializing button(s)\n");
    for (int i = 0; i < NUM_BUTTONS; ++i) 
    {
        buttons[i].gpioPinNum   = gpioPinNumbers[i];
        buttons[i].shortHoldDelayMs  = DEFAULT_SHORT_HOLD_DELAY_MS;
        buttons[i].timeHeldMs   = 0;
        buttons[i].isPressed    = 0;
        bool isOutput = false;
        GPIO_pinMode(buttons[i].gpioPinNum, isOutput); // goes into BBG file system and writes "in" to the direction.
    }
    //create new thread to check state of all buttons every BUTTON_SAMPLE_RATE_MS
    pthread_create(&readLoop, NULL, &runSampleLoop, NULL);
}

void Button_cleanupButtons()
{
    pthread_join(readLoop, NULL);
    for (int i = 0; i < NUM_BUTTONS; ++i) {
        //set all button data to be invalid for soft resets
        buttons[i].gpioPinNum   = -1;
        buttons[i].shortHoldDelayMs  = -1;
        buttons[i].timeHeldMs   = -1;
        buttons[i].isPressed    = -1;
    }
}

//loops through all buttons, checking/updating each of their states
static void* runSampleLoop()
{
    while (KillSignal_getIsRunning())
    {
        for (int i = 0; i < NUM_BUTTONS; ++i) 
        {
            struct button_t *b = &buttons[i];
            b->isPressed = GPIO_getValue(b->gpioPinNum);
            b->timeHeldMs = b->isPressed ? b->timeHeldMs + BUTTON_SAMPLE_RATE_MS : 0;

            //printf("is button[%d] pressed? %d\n", i, b->isPressed);
            //printf("is button[%d] held? %d, time held: %d\n", i, Button_isShortHeld(i), b->timeHeldMs);
        }
        delayMs(BUTTON_SAMPLE_RATE_MS);
    }
    return NULL;
}



int Button_isPressed(enum buttons button) { return buttons[button].isPressed && !Button_isShortHeld(button); }
int Button_isShortHeld(enum buttons button) { return buttons[button].timeHeldMs > buttons[button].shortHoldDelayMs; }
int Button_isLongHeld(enum buttons button) { return buttons[button].timeHeldMs > buttons[button].longHoldDelayMs; }
int Button_getTimeHeld(enum buttons button) {  return buttons[button].timeHeldMs; }

int Button_getShortHoldDelay(enum buttons button) { return buttons[button].shortHoldDelayMs; }
void Button_setShortHoldDelay(enum buttons button, int shortHoldDelayMs) { buttons[button].shortHoldDelayMs = shortHoldDelayMs; }

int Button_getLongHoldDelay(enum buttons button) { return buttons[button].longHoldDelayMs; }
void Button_setLongHoldDelay(enum buttons button, int longHoldDelayMs) { buttons[button].longHoldDelayMs = longHoldDelayMs; }
