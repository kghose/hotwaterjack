#include <stdio.h>

#include "data.h"

// const char *data_header[7] = {"tstat", "hot_in", "hot_out", "boiler_in", "boiler_out", "intake", "exhaust"};
const char *data_header[vars] = {"hot_in", "hot_out", "boiler_in", "boiler_out"};
const uint8_t temp_offset = 20; //-20F = 0 in the data

uint8_t *next_writable_row(BoilerData *boiler_data)
// Return row vector to write to. Internally, increments array index with loop around.
{
    uint16_t idx = boiler_data->last_index;
    boiler_data->last_index++;
    if (boiler_data->last_index++ == max_reading_index)
    {
        boiler_data->last_index = 0;
    }
    return boiler_data->data[idx];
}


DataChunks get_data_chunks(const BoilerData *boiler_data, uint16_t samples)
// Return data chunks for most recent N samples
{
    if (samples <= boiler_data->last_index)
    {
        return (DataChunks){
            .start = {boiler_data->last_index - samples, 0},
            .end = {boiler_data->last_index, 0},
            .has_two_chunks = 0};
    }
    else
    {
        return (DataChunks){
            .start = {max_reading_index - (samples - boiler_data->last_index), 0},
            .end = {max_reading_index, boiler_data->last_index},
            .has_two_chunks = 1};
    }
}

size_t human_readable(const uint8_t row[vars], char *out)
// Return row in human readable format
{
    size_t pos = 0;
    for (size_t i = 0; i < vars; i++)
    {
        pos += sprintf(&out[pos], "%s: %d\n", data_header[i], row[i]);
    }
    out[pos] = 0;
    return pos;
}