#pragma once

#include <pthread.h>

#define LOG_QUEUE_SIZE 100
#define LOG_STR_LEN 256

/** \brief The Log Buffer
 *
 * Allows to memorize messages in a circular queue for the
 * logging thread to consume
 */
typedef struct LogQueue {
    char message_queue[LOG_QUEUE_SIZE][LOG_STR_LEN]; /*!< The read circular queue */
    int head; /*!< Index of the head of the queue */
    int tail; /*!< Index of the tail of the queue */
    pthread_mutex_t lock; /*!< Lock to avoid race conditions */
    pthread_cond_t cond; /*!< Condition to signal between the logMessage function and the logging thread */
} LogQueue;

void initLogQueue(void);

void logMessage(const char* message);

void* loggingThread(void* arg);

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERR 3

#if !defined(LOG_LEVEL)
#define LOG_LEVEL LOG_LEVEL_ERR
#endif

#define LOG__XSTR(x) #x
#define LOG__STR(x) LOG__XSTR(x)

#define LOG_STRING(file, line, level, message) \
    file ": " line " | " level " - " message "\n"

#define LOG(T, message)                                                    \
    {                                                                      \
        logMessage(LOG_STRING(__FILE__, LOG__STR(__LINE__), #T, message)); \
    }

#if LOG_LEVEL == LOG_LEVEL_DEBUG
#define LOG_DEBUG(message) LOG([Debug], message);
#else
#define LOG_DEBUG(message)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO(message) LOG([Info], message);
#else
#define LOG_INFO(message)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARN
#define LOG_WARN(message) LOG([Warn], message);
#else
#define LOG_WARN(message)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERR
#define LOG_ERR(message) LOG([ERR], message);
#else
#define LOG_ERR(message)
#endif
