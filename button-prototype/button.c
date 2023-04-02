#include "button.h"
#include "GPIO.h"
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#define DEFAULT_SHORT_HOLD_DELAY_MS 200
#define DEFAULT_LONG_HOLD_DELAY_MS 500
#define SAMPLE_RATE_MS 20

static void delayMs(long long ms);
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
    //this will be true for up to SAMPLE_RATE_MS + jitter
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
void initButtons(int* gpioPinNumbers, int numButtons)
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
    //create new thread to check state of all buttons every SAMPLE_RATE_MS
    pthread_create(&readLoop, NULL, &runSampleLoop, NULL);
}

void cleanupButtons()
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
    while (isRunning)
    {
        for (int i = 0; i < NUM_BUTTONS; ++i) 
        {
            struct button_t *b = &buttons[i];
            b->isPressed = GPIO_getValue(b->gpioPinNum);
            b->timeHeldMs = b->isPressed ? b->timeHeldMs + SAMPLE_RATE_MS : 0;

            //printf("is button[%d] pressed? %d\n", i, b->isPressed);
            //printf("is button[%d] held? %d, time held: %d\n", i, isShortHeld(i), b->timeHeldMs);
        }
        delayMs(SAMPLE_RATE_MS);
    }
    cleanupButtons();
    return NULL;
}

static void delayMs(long long ms)
{
    const long long NS_PER_MS = 1000000;
    const long long NS_PER_SECOND = 1000000000;
    long long ns = ms * NS_PER_MS;
    struct timespec ts = {ns / NS_PER_SECOND, ns % NS_PER_SECOND};
    nanosleep(&ts, (struct timespec *)NULL);
}

int isPressed(enum buttons button) { return buttons[button].isPressed && !isShortHeld(button); }
int isShortHeld(enum buttons button) { return buttons[button].timeHeldMs > buttons[button].shortHoldDelayMs; }
int isLongHeld(enum buttons button) { return buttons[button].timeHeldMs > buttons[button].longHoldDelayMs; }
int getTimeHeld(enum buttons button) {  return buttons[button].timeHeldMs; }

int getShortHoldDelay(enum buttons button) { return buttons[button].shortHoldDelayMs; }
void setShortHoldDelay(enum buttons button, int shortHoldDelayMs) { buttons[button].shortHoldDelayMs = shortHoldDelayMs; }

int getLongHoldDelay(enum buttons button) { return buttons[button].longHoldDelayMs; }
void setLongHoldDelay(enum buttons button, int longHoldDelayMs) { buttons[button].longHoldDelayMs = longHoldDelayMs; }
