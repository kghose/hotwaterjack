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
    if (boiler_data->last_index == max_reading_index)
    {
        boiler_data->last_index = 0;
    }
    boiler_data->total_samples++;
    return boiler_data->data[idx];
}

DataChunks get_data_chunks(const BoilerData *boiler_data, uint16_t samples)
// Return data chunks for most recent N samples
{
    if (samples <= boiler_data->last_index)
    {
        return (DataChunks){
            .start = {(boiler_data->last_index - samples) * vars, 0},
            .end = {boiler_data->last_index * vars, 0},
            .has_two_chunks = 0};
    }
    else
    {
        return (DataChunks){
            .start = {(max_reading_index - (samples - boiler_data->last_index)) * vars, 0},
            .end = {max_reading_index * vars, boiler_data->last_index * vars},
            .has_two_chunks = 1};
    }
}

size_t boiler_info(const BoilerData *boiler_data, char *out)
// Return debugging information about data collection
{
    size_t pos = 0;
    pos += sprintf(&out[pos], "Found %d temp sensors\n", boiler_data->tsensor_count);
    for (size_t i = 0; i < boiler_data->tsensor_count; i++)
    {
        pos += sprintf(&out[pos], "%d: ", i);
        for (size_t j = 0; j < 8; j++)
        {
            pos += sprintf(&out[pos], "%02x ", boiler_data->tsensor_address[i][j]);
        }
        pos += sprintf(&out[pos], "\n");
    }
    pos += sprintf(&out[pos], "Read %d samples\n", boiler_data->total_samples);
    pos += sprintf(&out[pos], "Sampling every %3.2f seconds\n", sample_dt_us / 1000000.0);
    pos += sprintf(&out[pos], "Rolling buffer size is %d samples\n", max_reading_index);
    out[pos] = 0;
    return pos;
}

uint8_t *latest_sample(const BoilerData *boiler_data)
// Just the latest sample
{
    if (boiler_data->last_index == 0)
    {
        return boiler_data->data[max_reading_index];
    }
    else
    {
        return boiler_data->data[boiler_data->last_index - 1];
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