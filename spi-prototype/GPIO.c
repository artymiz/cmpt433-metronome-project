#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "GPIO.h"

#define FILENAME_FMTSTR "/sys/class/gpio/gpio%d/%s"
#define MAX_FILENAME_LEN 100

#define COMMAND_CONFIG_PIN "config-pin p%d.%d gpio > /dev/null"
#define MAX_COMMAND_LEN 100

void GPIO_usePin(gpioInfo_t *gpioInfo, char *pinMode)
{
    char filename[MAX_FILENAME_LEN];
    sprintf(filename, FILENAME_FMTSTR, gpioInfo->gpioNumber, "direction");
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "GPIO ERROR: failed to open %s\n", filename);
        exit(1);
    }
    int charcount = fprintf(file, pinMode);
    if (charcount <= 0) {
        fprintf(stderr, "GPIO ERROR: failed to write %s\n", filename);
        exit(1);
    }
    fclose(file);

    char command[MAX_COMMAND_LEN];
    sprintf(command, COMMAND_CONFIG_PIN, gpioInfo->pin.header, gpioInfo->pin.number);
    system(command);
}

int GPIO_getValue(gpioInfo_t *gpioInfo)
{
    char filename[MAX_FILENAME_LEN];
    sprintf(filename, FILENAME_FMTSTR, gpioInfo->gpioNumber, "value");
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "GPIO ERROR: failed to open %s\n", filename);
        exit(1);
    }
    char first_character = fgetc(file);
    fclose(file);
    return first_character - '0';
}

void GPIO_setValue(gpioInfo_t *gpioInfo, bool isHigh)
{
    char filename[MAX_FILENAME_LEN];
    sprintf(filename, FILENAME_FMTSTR, gpioInfo->gpioNumber, "value");
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "GPIO ERROR: failed to open %s\n", filename);
        exit(1);
    }
    char *toWrite = isHigh ? "1" : "0";
    int itemsWritten = fwrite(toWrite, 1, 1, file);
    if (itemsWritten != 1) {
        fprintf(stderr, "GPIO ERROR: failed to write %s\n", filename);
        exit(1);
    }
    fclose(file);
}
