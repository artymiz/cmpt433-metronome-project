#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "GPIO.h"

#define FILENAME_FMTSTR "/sys/class/gpio/gpio%d/%s"
#define MAX_FILENAME_LEN 100

void GPIO_usePin(gpioInfo_t *gpioInfo, char *direction)
{
    char filename[MAX_FILENAME_LEN];
    sprintf(filename, FILENAME_FMTSTR, gpioInfo->gpioNumber, "direction");
    FILE *directionFile = fopen(filename, "w");
    if (!directionFile) {
        fprintf(stderr, "GPIO ERROR: failed to open %s\n", filename);
        exit(1);
    }
    int charcount = fprintf(directionFile, direction);
    if (charcount <= 0) {
        fprintf(stderr, "GPIO ERROR: failed to write %s\n", filename);
        exit(1);
    }
    fclose(directionFile);
}

int GPIO_getValue(gpioInfo_t *gpioInfo)
{
    char filename[MAX_FILENAME_LEN];
    sprintf(filename, FILENAME_FMTSTR, gpioInfo->gpioNumber, "value");
    FILE *valueFile = fopen(filename, "r");
    if (!valueFile) {
        fprintf(stderr, "GPIO ERROR: failed to open %s\n", filename);
        exit(1);
    }
    char first_character = fgetc(valueFile);
    fclose(valueFile);
    int value = first_character - '0';
    return value;
}

void GPIO_setValue(gpioInfo_t *gpioInfo, bool isHigh)
{
    char filename[MAX_FILENAME_LEN];
    sprintf(filename, FILENAME_FMTSTR, gpioInfo->gpioNumber, "value");
    FILE *valueFile = fopen(filename, "w");
    if (!valueFile) {
        fprintf(stderr, "GPIO ERROR: failed to open %s\n", filename);
        exit(1);
    }
    char *toWrite = isHigh ? "1" : "0";
    int itemsWritten = fwrite(toWrite, 1, 1, valueFile);
    if (itemsWritten != 1) {
        fprintf(stderr, "GPIO ERROR: failed to write %s\n", filename);
        exit(1);
    }
    fclose(valueFile);
}
