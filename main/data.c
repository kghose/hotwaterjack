#include "data.h"

const char *data_header[7] = {"tstat", "hot_in", "hot_out", "boiler_in", "boiler_out", "intake", "exhaust"};
const uint8_t temp_offset = 20; //-20F = 0 in the data
