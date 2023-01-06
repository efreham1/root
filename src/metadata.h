/// \file

#include <stdbool.h>

#ifndef __metadata__
#define __metadata__

typedef void *metadata_t;
/// @brief checks weather a metadata is a format vector
/// @param mdata the metadata
/// @return true if the first 2 idexes in meatdata is 0
bool is_format_vector (metadata_t mdata);
bool is_data_size (metadata_t mdata);

/// @brief checks weather a metadata is a forwarding address
/// @param mdata the metadata
/// @return true if the first 2 idexes in meatdata is not 0
bool is_forward_address (metadata_t mdata);

/// @brief sets the forwarding adress
/// @param ptr pointer to set
/// @return a forwarding adress
metadata_t set_forward_address (void *ptr);

/// @brief gets the forward adress
/// @param mdata where to fin the adress
/// @return forward adress as a void*
void *get_forward_address (metadata_t mdata);

/// @brief gets the size of data
/// @param mdata the data
/// @return teh size
unsigned int get_data_size (metadata_t mdata);

/// @brief sets the data size
/// @param size size
/// @return the set data
metadata_t set_data_size (unsigned int size);

/// @brief gets the format vector from metadata
/// @param mdata the metadata
/// @param idx the index from where the boolean should be read
/// @return a the corresponding boolean
bool get_format_vector_idx (metadata_t mdata, int idx);

/// @brief sets the format vector
/// @param format_vector adress to the format vector
/// @param len 
/// @return format vector as metadata
metadata_t set_format_vector (bool *format_vector, unsigned int len);

/// @brief finds the size of a format vector
/// @param md the metadata
/// @return size of the metadata
unsigned int get_size_format_vector (metadata_t md);

/// @brief gets the visitationbit from the metadata
/// @param md the metadata
/// @return the visitionbit
bool get_visitation_bit (metadata_t md);

/// @brief sets the visitation_bit
/// @param md metadata
/// @param visitation_bit visitationbit
/// @return metadata with visitationbit set
metadata_t set_visitation_bit (metadata_t md, bool visitation_bit);

#endif