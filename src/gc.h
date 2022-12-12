/// \file

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef __gc__
#define __gc__

/// The opaque data type holding all the heap data
typedef struct external_heap heap_t;

/// Create a new heap
///
/// \param No_pages the total number of pages, has to be larger than 2
/// \param unsafe_stack true if pointers on the stack are to be considered unsafe pointers
/// \param gc_threshold the memory pressure at which gc should be triggered (1.0 = full memory)
/// \return the new heap
heap_t *h_init(unsigned int No_pages, bool unsafe_stack, float gc_threshold);

/// Delete a heap.
///
/// \param h the heap
void h_delete(heap_t *h);

/// Delete a heap and trace, killing off stack pointers.
///
/// \param h the heap 
/// \param dbg_value a value to be written into every pointer into h on the stack
void h_delete_dbg(heap_t *h, void *dbg_value);

/// Allocate a new object on a heap with a given format string.
///
/// Valid characters in format strings are:
/// - 'i' -- for sizeof(int) bytes 'raw' data
/// - 'l' -- for sizeof(long) bytes 'raw' data
/// - 'f' -- for sizeof(float) bytes 'raw' data
/// - 'c' -- for sizeof(char) bytes 'raw' data
/// - 'd' -- for sizeof(double) bytes 'raw' data
/// - '*' -- for a sizeof(void *) bytes pointer value
/// - '\0' -- null-character terminates the format string
///
/// \param h the heap
/// \param layout the format string
/// \return the newly allocated object
///
/// Note: the heap does *not* retain an alias to layout.
void *h_alloc_struct(heap_t *h, char *layout);

/// Allocate a new object on a heap with a given size.
///
/// Objects allocated with this function will *not* be 
/// further traced for pointers. 
///
/// \param h the heap
/// \param bytes the size in bytes
/// \return the newly allocated object
void *h_alloc_data(heap_t *h, unsigned int bytes);

/// Manually trigger garbage collection.
///
/// Garbage collection is otherwise run when an allocation is
/// impossible in the available consecutive free memory.
///
/// \param h the heap
/// \return the number of bytes collected
unsigned int h_gc(heap_t *h);

/// Manually trigger garbage collection with the ability to 
/// override the setting for how stack pointers are treated. 
/// 
/// Garbage collection is otherwise run when an allocation is
/// impossible in the available consecutive free memory.
///
/// \param h the heap
/// \param unsafe_stack true if pointers on the stack are to be considered unsafe pointers
/// \return the number of bytes collected
unsigned int h_gc_dbg(heap_t *h, bool unsafe_stack);

/// Returns the available free memory. 
///
/// \param h the heap
/// \return the available free memory. 
unsigned int h_avail(heap_t *h);

/// Returns the bytes currently in use by user structures. This
/// should not include the collector's own meta data. Notably,
/// this means that h_avail + h_used will not equal the size of
/// the heap passed to h_init.
/// 
/// \param h the heap
/// \return the bytes currently in use by user structures. 
unsigned int h_used(heap_t *h);

#endif
