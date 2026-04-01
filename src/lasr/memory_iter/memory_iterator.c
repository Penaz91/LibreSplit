#include "memory_iterator.h"
#include "src/lasr/utils.h"
#include "src/logging.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

const ptrdiff_t MEMORY_WINDOW_SIZE = 0x100000;

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
    return iter;
}

int mem_next(uint8_t** buffer, size_t* buffer_size, MemoryIterator* iterator, uint8_t* err)
{
    ptrdiff_t window_size = MEMORY_WINDOW_SIZE;
    bool last_iter = false;
    assert(iterator->cursor >= iterator->start);
    if (iterator->cursor >= iterator->end) {
        // We passed the end in the last iteration. Stop.
        return 0;
    }
    if (iterator->cursor + window_size > iterator->end) {
        // We're at the last iteration and the window size is too big
        window_size = iterator->end - iterator->cursor;
        last_iter = true;
    }
    // Reallocate buffer for reading
    *buffer = (uint8_t*)realloc(*buffer, window_size);
    if (buffer == NULL) {
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
        // Memory read error
        // TODO: [Penaz] [2026-04-01] Change this to an enum
        *err = 2;
    }
    // Nothing more to read or error has happened.
    return 0;
}
