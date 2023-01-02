/// \file

#include <stdbool.h>

#ifndef __metadata__
#define __metadata__

typedef void * metadata_t;

bool is_format_vector(metadata_t mdata);

bool is_data_size(metadata_t mdata);

bool is_forward_address(metadata_t mdata);

metadata_t set_forward_address(void *ptr);

void *get_forward_address(metadata_t mdata);

unsigned int get_data_size(metadata_t mdata);

metadata_t set_data_size(unsigned int size);

/// @brief 
/// @param mdata 
/// @param idx the index from where the boolean should be read
/// @return a the corresponding boolean
bool get_format_vector_idx(metadata_t mdata, int idx);

metadata_t set_format_vector(bool *format_vector, unsigned int len);

unsigned int get_size_format_vector(metadata_t md);

bool get_visitation_bit(metadata_t md);

metadata_t set_visitation_bit(metadata_t md, bool visitation_bit);

#endif