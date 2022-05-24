#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <stdbool.h>
#include "time.h"

const char *data_header[7];
const uint8_t temp_offset;
typedef struct
{
    uint8_t tstat;
    uint8_t t_hot_in;
    uint8_t t_hot_out;
    uint8_t t_boiler_in;
    uint8_t t_boiler_out;
    uint8_t t_intake;
    uint8_t t_exhaust;
} Reading;

void _print_reading(const Reading *, bool csv, char *out);

#define max_reading_index 1440
typedef struct
{
    struct tm date;
    Reading data[max_reading_index];
    uint16_t index;
} DayData;

#define max_day_index 7
typedef struct
{
    DayData day[max_day_index];
    uint8_t index;
} BoilerData;

void print_header(char *out);
void print_reading(const DayData *, uint8_t day_index, uint16_t reading_index, bool csv, char *out);

#endif