#ifndef DATA_H
#define DATA_H

#include <stdint.h>
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

typedef struct
{
    uint16_t start[2];
    uint16_t end[2];
    uint8_t has_two_chunks;
} DataChunks;

DataChunks get_data_chunks(const BoilerData *, uint16_t);
// Return data chunks for most recent N samples

uint8_t *latest_sample(const BoilerData *);
// Just the latest sample

size_t human_readable(const uint8_t row[vars], char *out);
// Return row in human readable format

#endif