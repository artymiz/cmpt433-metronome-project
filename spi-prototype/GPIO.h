/**
 * Usage for output pin: 
 * int pinNumber = 49;
 * bool isOutput = true;
 * GPIO_pinMode(pinNumber, isOutput); // goes into BBG file system and writes "out" to the direction.
 * GPIO_setValue(pinNumber);
 * 
 * Usage for input pin:
 * int pinNumber = 49;
 * bool isOutput = false;
 * GPIO_pinMode(pinNumber, isOutput); // goes into BBG file system and writes "in" to the direction.
 * int value = GPIO_getValue(pinNumber);
 */

#include <stdint.h>
#include <stdbool.h>

int GPIO_getValue(int pin);
void GPIO_pinMode(int pin, bool isOutput);
void GPIO_setValue(int pin, bool isHigh);
