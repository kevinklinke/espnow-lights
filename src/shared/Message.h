#pragma once

#include <stdint.h>

// Common ESP-NOW message payload between transmitter and receiver.
enum class MessageType : uint8_t {
    NORMAL = 1,
    OPPOSITE = 2,
};

struct Message {
    MessageType type;
    uint8_t destination_id;
    uint8_t padding[2];
    uint32_t seq_num;
};

static_assert(sizeof(Message) == 8, "Message must be 8 bytes");
