/**
 * \file src/buffer.c
 *
 * `buffer.c` implements a circular buffer for generic data types.
 *
 * \author H Paterson <harley.paterson@postgrad.otago.ac.nz>
 * \copyright BSL-1.0
 * \date February 2020
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Internal state for a circular buffer.
 */
struct CircularBuffer
{
    /** Data store for the buffer. */
    uint8_t* data;

    /** Allocated length of `data`, in bytes. */
    size_t length;

    /** Index to make the next read from. */
    size_t readIndex;

    /** Index to make the next write to. */
    size_t writeIndex;
};

/** Opaque handle to a circular buffer. */
typedef struct CircularBuffer* CircularBuffer;

/**
 * Allocate and initialise a circular buffer.
 *
 * \param length Length, in bytes, to allocate for the buffer.
 * \return A handle to the new buffer, or `NULL` on failure.
 */
CircularBuffer circularBufferAlloc(const size_t length)
{
    CircularBuffer buffer;
    assert(length > 0);
    buffer = malloc(sizeof (struct CircularBuffer));
    if (buffer == NULL) {
        return NULL;
    }
    buffer->length = length;
    buffer->readIndex = 0;
    buffer->writeIndex = 0;
    buffer->data = malloc(length);
    if (buffer->data == NULL) {
        circularBufferFree(buffer);
        return NULL;
    }
    return buffer;
}

/**
 * Deallocate and free a circular buffer.
 *
 * \param buffer Circular buffer to deallocate.
 */
void circularBufferFree(CircularBuffer buffer)
{
    if (buffer == NULL) {
        return;
    }
    if (buffer->data != NULL) {
        free(buffer->data);
    }
    /* Guard against reads from a dangling pointer. */
    buffer->length = 0;
    buffer->readIndex = 0;
    buffer->writeIndex = 0;
    buffer->data = NULL;
    return;
}

/**
 * Tests if a circular buffer is empty.
 *
 * A circular buffer is empty if it has no more data ready to read.
 *
 * \param buffer The buffer to query.
 * \return `true` if the buffer is empty; `false` otherwise.
 */
bool circularBufferIsEmpty(CircularBuffer buffer)
{
    if (buffer->readIndex == buffer->writeIndex) {
        return true;
    }
    return false;
}
