/** \file logging.c
 * Logger Rollster
 *
 * Asynchronous Logging Library for LibreSplit based on threads, circular queues,
 * hopes and dreams.
 */
#include "logging.h"

#include <linux/prctl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/prctl.h>

/*! The log queue, used as buffer */
LogQueue logQueue;
/*! Atomic bool used to keep the thread active, might be used for clean closing in future */
atomic_bool logging_active;

/**
 * Initializes the log queue, ready to receive messages
 */
void initLogQueue(void)
{
    logQueue.head = 0;
    logQueue.tail = 0;
    pthread_mutex_init(&logQueue.lock, NULL);
    pthread_cond_init(&logQueue.cond, NULL);
    logging_active = 1;
}

/**
 * Underlying function to all the LOG_* macros
 *
 * Works as a producer.
 *
 * @param[in] fmt The message to print in the log or the format string
 */
void logMessage(const char* fmt, ...)
{
    // Lock the mutex for writing
    pthread_mutex_lock(&logQueue.lock);
    // If the queue is full, wait (bottleneck)
    while ((logQueue.tail + 1) % LOG_QUEUE_SIZE == logQueue.head) {
        pthread_cond_wait(&logQueue.cond, &logQueue.lock);
    }
    // There is space in the queue, add a new message
    va_list args;
    va_start(args, fmt);
    vsnprintf(logQueue.message_queue[logQueue.tail], LOG_STR_LEN, fmt, args);
    va_end(args);
    logQueue.tail = (logQueue.tail + 1) % LOG_QUEUE_SIZE;

    // Signal a possibly waiting logging thread
    pthread_cond_signal(&logQueue.cond);
    // Unlock the mutex
    pthread_mutex_unlock(&logQueue.lock);
}

/**
 * Pops a message from the log queue, writing it into console
 * and the log file. Just a utility.
 *
 * @param logfile The File pointer to write into
 */
void pop_message(FILE* logfile)
{
    // Remove a message from the queue
    // We don't empty the whole queue to avoid being a bottleneck for the
    // addition of new messages.
    // Log to console
    printf("%s\n", logQueue.message_queue[logQueue.head]);
    // Log to file
    fprintf(logfile, "%s\n", logQueue.message_queue[logQueue.head]);
    // Flush the file immediately to disk, in case something crashes
    fflush(logfile);
    logQueue.head = (logQueue.head + 1) % LOG_QUEUE_SIZE;
}

/**
 * The logging thread, writes the queued messages in the log.
 *
 * Works as a consumer
 *
 * @param arg Unused.
 */
void* loggingThread(void* arg)
{
    prctl(PR_SET_NAME, "LS Logger", 0, 0, 0);
    FILE* logfile = fopen("libresplit.log", "a");
    if (!logfile) {
        perror("Failed to open log file");
        return NULL;
    }
    while (atomic_load(&logging_active)) {
        // Lock the mutex for reading
        pthread_mutex_lock(&logQueue.lock);
        // If the queue is empty, wait
        while (logQueue.head == logQueue.tail) {
            pthread_cond_wait(&logQueue.cond, &logQueue.lock);
            // We got signalled by the main thread to close up
            if (!atomic_load(&logging_active)) {
                break;
            }
        }
        pop_message(logfile);
        // Unlock the mutex
        pthread_mutex_unlock(&logQueue.lock);
    }
    // We're closing the thread, close the file
    fclose(logfile);
    return 0;
}

/**
 * Function to close the logger thread.
 *
 * This is needed because while we're closing, we might be waiting for
 * the queue to fill up. If that's the case, we signal the thread to continue
 * after setting logging_active to false.
 */
void close_logger()
{
    // If we're stuck waiting for the buffer to fill
    atomic_store(&logging_active, 0);
    if (logQueue.head == logQueue.tail) {
        // Signal the logging thread to continue, so to hit the closing condition
        pthread_cond_signal(&logQueue.cond);
    }
}
