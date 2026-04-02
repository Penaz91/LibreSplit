#include "memory_iterator.h"
#include "src/lasr/utils.h"
#include "src/logging.h"
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const size_t MEMORY_WINDOW_SIZE = 0x100000; /*!< The size of the memory chunks to be read*/

/**
 * Creates a new chunked Memory Iterator.
 *
 * @param[in] pid The process id to scan for.
 * @param[in] start The start address to start the scan from.
 * @param[in] end The end address onto where to finish the scan.
 * @param[in] overlap The amount of overlap between two successive iterations.
 *
 * @returns A pointer to a new memory iterator.
 */
MemoryIterator* mem_iterator_new(pid_t pid, uintptr_t start, uintptr_t end, uintptr_t overlap)
{
    MemoryIterator* iter = malloc(sizeof(MemoryIterator));
    if (iter == NULL) {
        return NULL;
    }
    iter->pid = pid;
    iter->start = start;
    iter->end = end;
    iter->overlap = overlap;
    iter->cursor = start;
    iter->last_cursor = start;
    return iter;
}

/**
 * Reads the next chunk of memory.
 *
 * @param[out] buffer The buffer to copy memory into.
 * @param[out] buffer_size The size of the buffer that is allocated by the iterator.
 * @param[in] iterator The pointer to the MemoryIterator used.
 * @param[out] err The error code returned to the calling function.
 *
 * @returns 1 if there has been a memory read, 0 otherwise.
 */
int mem_next(uint8_t** buffer, size_t* buffer_size, MemoryIterator* iterator, uint8_t* err)
{
    assert(iterator != NULL);
    size_t window_size = MEMORY_WINDOW_SIZE;
    bool last_iter = false;
    assert(iterator->cursor >= iterator->start);
    if (iterator->cursor >= iterator->end) {
        // We passed the end in the last iteration. Stop.
        return 0;
    }
    if (iterator->cursor + window_size > iterator->end) {
        // We're at the last iteration and the window size is too big
        // FIXME: [Penaz] [2026-04-02] I received a short memory read while testing, this
        // ^ might not work
        window_size = (size_t)(iterator->end - iterator->cursor);
        last_iter = true;
    }
    // Reallocate buffer for reading
    *buffer = (uint8_t*)realloc(*buffer, window_size);
    if (*buffer == NULL) {
        // Cannot allocate buffer, stop immediately
        LOG_WARN("Unable to allocate memory read buffer");
        *err = 1;
        return 0;
    }
    *buffer_size = window_size;
    // Read memory
    struct iovec local_iov = { *buffer, window_size };
    struct iovec remote_iov = { (void*)iterator->cursor, window_size };
    ssize_t nread = process_vm_readv(iterator->pid, &local_iov, 1, &remote_iov, 1, 0);
    if (nread == (ssize_t)window_size) {
        iterator->last_cursor = iterator->cursor;
        // If read is okay, move the cursor forward
        iterator->cursor += window_size;
        if (!last_iter) {
            // If we're not done, move it backwards a little to account for
            // Matches between chunks
            iterator->cursor -= iterator->overlap;
        }
        // There are more things to read
        return 1;
    } else {
        if (nread < 0) {
            LOG_DEBUGF("Memory read error, errno is %d (%s)", errno, strerror(errno));
            LOG_DEBUGF("Cursor: %x, Start: %x, End: %x, Overlap: %x", iterator->cursor, iterator->start, iterator->end, iterator->overlap);
            *err = 3;
        } else {
            // Memory read error
            LOG_WARNF("Memory read error: nread=%zd, expected=%td", nread, window_size);
            *err = 2;
        }
    }
    // Nothing more to read or error has happened.
    return 0;
}
