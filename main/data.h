#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <stdbool.h>
#include "time.h"

#define vars 4
const char *data_header[vars];
const uint8_t temp_offset;

#define max_reading_index 10080 // 60min/hr * 24hr/day * 7 days (1 sample/min)
typedef struct
{
    uint8_t data[max_reading_index][vars];
    uint16_t last_index;
} BoilerData;

uint8_t *next_writable_row(BoilerData *);
// Return row vector to write to. Internally, increments array index with loop around.

uint16_t latest_row(const BoilerData *);
// last_index - 1 with loop around

uint8_t const *read_row_and_decrement(const BoilerData *, uint16_t *);
// Return this row vector and return the previous index, with loop around.

size_t human_readable(const uint8_t row[vars], char *out);
// Return row in human readable format

#endif