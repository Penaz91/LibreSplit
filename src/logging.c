/** \file logging.c
 * Logger Rollster
 *
 * Asynchronous Logging Library for LibreSplit based on threads, circular queues,
 * hopes and dreams.
 */
#include "logging.h"

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*! The log queue, used as buffer */
LogQueue logQueue;
/*! Atomic bool used to keep the thread active, might be used for clean closing in future */
atomic_bool logging_active = true;

/**
 * Initializes the log queue, ready to receive messages
 */
void initLogQueue(void)
{
    logQueue.head = 0;
    logQueue.tail = 0;
    pthread_mutex_init(&logQueue.lock, NULL);
    pthread_cond_init(&logQueue.cond, NULL);
}

/**
 * Underlying function to all the LOG_* macros
 *
 * Works as a producer.
 *
 * @param[in] message The message to print in the log
 */
void logMessage(const char* message)
{
    // Lock the mutex for writing
    pthread_mutex_lock(&logQueue.lock);
    // If the queue is full, wait (bottleneck)
    while ((logQueue.tail + 1) % LOG_QUEUE_SIZE == logQueue.head) {
        pthread_cond_wait(&logQueue.cond, &logQueue.lock);
    }
    // There is space in the queue, add a new message
    strncpy(logQueue.message_queue[logQueue.tail], message, LOG_STR_LEN);
    logQueue.tail = (logQueue.tail + 1) % LOG_QUEUE_SIZE;

    // Signal a possibly waiting logging thread
    pthread_cond_signal(&logQueue.cond);
    // Unlock the mutex
    pthread_mutex_unlock(&logQueue.lock);
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
        }
        // Empty a message from the queue
        // We don't empty the whole queue to avoid being a bottleneck for the
        // addition of new messages.
        // Log to console
        printf("%s\n", logQueue.message_queue[logQueue.head]);
        // Log to file
        fprintf(logfile, "%s\n", logQueue.message_queue[logQueue.head]);
        logQueue.head = (logQueue.head + 1) % LOG_QUEUE_SIZE;
        // Unlock the mutex
        pthread_mutex_unlock(&logQueue.lock);
    }
    // We're closing the thread, close the file
    fclose(logfile);
    return NULL;
}
