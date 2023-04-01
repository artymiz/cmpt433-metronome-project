#include "button.h"
#include "GPIO.h"
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#define DEFAULT_HOLD_DELAY_MS 200
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
    //how long the button needs to be held until it is considered held down/long-pressed
    int holdDelayMs;
    //this will be true for up to SAMPLE_RATE_MS + jitter
    int isPressed;
    //this will be true for as long as the button is being sampled as true for longer than holdDelayMs
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
        buttons[i].holdDelayMs  = DEFAULT_HOLD_DELAY_MS;
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
        buttons[i].holdDelayMs  = -1;
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
            //printf("is button[%d] held? %d, time held: %d\n", i, isHeld(i), b->timeHeldMs);
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

int isPressed(enum buttons button) { return buttons[button].isPressed && !isHeld(button); }
int isHeld(enum buttons button) { return buttons[button].timeHeldMs > buttons[button].holdDelayMs; }
int getTimeHeld(enum buttons button) {  return buttons[button].timeHeldMs; }
int getHoldDelay(enum buttons button) { return buttons[button].holdDelayMs; }
void setHoldDelay(enum buttons button, int holdDelayMs) { buttons[button].holdDelayMs = holdDelayMs; }
