#ifndef DATA_H
#define DATA_H

#include <stdint.h>

#define vars 4
const char *data_header[vars];

typedef uint8_t SensorAddress[8];

#define sample_dt_us 60000000 // 1 sample every 1min
#define buffer_len 10080      // 60min/hr * 24hr/day * 7 days (1 sample/min)
typedef struct
{
    int16_t data[buffer_len][vars];
    uint16_t last_index;
    uint32_t total_samples;
    SensorAddress tsensor_address[vars];
    size_t tsensor_count;
} BoilerData;

int16_t *next_writable_row(BoilerData *);
// Return row vector to write to. Internally, increments array index with loop around.

typedef struct
{
    uint32_t start[2];
    uint32_t end[2];
    uint8_t has_two_chunks;
} DataChunks;

DataChunks get_data_chunks(const BoilerData *, uint16_t);
// Return data chunks for most recent N samples

size_t boiler_info(const BoilerData *, char *out);
// Return debugging information about data collection

const int16_t *latest_sample(const BoilerData *);
// Just the latest sample

size_t human_readable(const int16_t row[vars], char *out);
// Return row in human readable format

#endif