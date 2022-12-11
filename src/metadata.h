#include <stdbool.h>

typedef void * metadata_t;

bool is_format_vector(metadata_t mdata);

bool is_data_size(metadata_t mdata);

bool is_forward_address(metadata_t mdata);

metadata_t set_forward_address(void *ptr);

size_t get_data_size(metadata_t mdata);

metadata_t set_data_size(size_t size);

/// @brief 
/// @param mdata 
/// @param len a pointer to where the length should be written
/// @return a malloc'd array of booleans
bool *get_format_vector(metadata_t mdata, int *len);

metadata_t set_format_vector(bool *format_vector, size_t len);