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

uint16_t latest_row(const BoilerData *boiler_data)
// last_index - 1 with loop around
{
    if (boiler_data->last_index == 0)
    {
        return max_reading_index - 1;
    }
    else
    {
        return boiler_data->last_index - 1;
    }
}
uint8_t const *read_row_and_decrement(const BoilerData *boiler_data, uint16_t *index)
// Return this row vector and return the previous index, with loop around.
{
    uint16_t idx = *index;
    if (*index == 0)
    {
        *index = max_reading_index - 1;
    }
    else
    {
        (*index)--;
    }
    return boiler_data->data[idx];
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