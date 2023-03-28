#include <stdint.h>
#include <stdbool.h>

int GPIO_getValue(int pin);
void GPIO_usePin(int pin, char *pinMode);
void GPIO_setValue(int pin, bool isHigh);
