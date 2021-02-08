/**
 * \file src/buffer.c
 *
 * `buffer.c` implements a  buffer for generic data types.
 *
 * \see `include/networking/buffer.h` for detailed API documentation and
 * documentation comments.
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
#include "networking/buffer.h"

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

/** Allocate and initialise a buffer. */
Buffer bufferAlloc(const size_t length)
{
    Buffer buffer;
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
        bufferFree(buffer);
        return NULL;
    }
    return buffer;
}

/** Deallocate and free a buffer. */
void bufferFree(Buffer buffer)
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

/** Tests if a buffer is empty. */
bool bufferIsEmpty(Buffer buffer)
{
    if (buffer->readIndex == buffer->writeIndex) {
        return true;
    }
    return false;
}

/** Find how many items (not bytes) are in a buffer, ready to be consumed. */
size_t bufferItems(Buffer buffer, size_t itemSize)
{
    size_t bytesReady = 0;
    assert(buffer != NULL);
    assert(itemSize > 0);
    if (bufferIsEmpty(buffer)) {
        return 0;
    }
    if (buffer->writeIndex > buffer->readIndex) {
        bytesReady = buffer->writeIndex - buffer->readIndex;
    }
    else {
        bytesReady = buffer->length - (buffer->readIndex - buffer->writeIndex);
    }
    return bytesReady / itemSize;
}

/**
 * Read entries from the buffer.
 *
 * \param buffer The buffer to read from.
 * \param n The number of entries to read.
 * \param output Allocated memory to store the output in.
 * \return The number of items read from the buffer.
 */
size_t bufferRead(Buffer buffer, size_t n, void* output)
{
    assert(buffer != NULL);
    assert(n > 0);
    assert(output != NULL);
    size_t itemsRead = bufferItems(buffer);

}