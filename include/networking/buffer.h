/**
 * \file include/networking/buffer.h
 *
 * `buffer.h` declears functions and data types for a circular buffer.
 *
 * \author H Paterson <harley.paterson@postgrad.otago.ac.nz>
 * \copyright BSL-1.0
 * \date February 2021.
 */

#include <stddef.h>

/** Opaque handle to a buffer. */
typedef struct CircularBuffer* Buffer;

/**
 * Allocate and initialise a buffer.
 *
 * \param length Length, in bytes, to allocate for the buffer.
 * \return A handle to the new buffer, or `NULL` on failure.
 */
Buffer bufferAlloc(const size_t length);

/**
 * Deallocate and free a buffer.
 *
 * \param buffer Buffer to deallocate.
 */
void bufferFree(Buffer buffer);

/**
 * Tests if a buffer is empty.
 *
 * A buffer is empty if it has no more data ready to read.
 *
 * \param buffer The buffer to query.
 * \return `true` if the buffer is empty; `false` otherwise.
 */
bool bufferIsEmpty(Buffer buffer);

