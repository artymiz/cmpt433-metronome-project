#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t header;
    uint16_t number;
} hardwarePin_t;

typedef uint16_t gpioNumber_t;

typedef struct {
    hardwarePin_t pin;
    gpioNumber_t gpioNumber;
} gpioInfo_t;

int GPIO_getValue(gpioInfo_t*);
void GPIO_usePin(gpioInfo_t*, char *pinMode);
void GPIO_setValue(gpioInfo_t *gpioInfo, bool isHigh);
