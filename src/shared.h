#pragma once

#include <stddef.h>
#include <stdint.h>

#define LIBRESPLIT_SOCK_NAME "libresplit.sock"

void getXDGruntimeDir(char* buffer, size_t size);

typedef enum : uint8_t {
    CTL_CMD_START_SPLIT,
    CTL_CMD_STOP_RESET,
    CTL_CMD_CANCEL,
    CTL_CMD_UNSPLIT,
    CTL_CMD_SKIP,
    CTL_CMD_EXIT,
} CTLCommand;

typedef struct __attribute__((__packed__)) CTLMessage {
    uint32_t length; // Size of message in bytes
    uint8_t message[];
} CTLMessage;
