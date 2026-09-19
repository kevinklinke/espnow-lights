#pragma once

#include <stdint.h>

// Minimal trigger packet used by simple transmitters to nudge receivers.
// Receivers interpret triggers according to their local effect logic.
enum class TriggerCmd : uint8_t {
    FLASH = 1, // brief bright flash
    BAR = 2,   // start a solid-bar wave (arg used as intensity/palette id)
};

struct Trigger {
    TriggerCmd cmd;          // command to execute
    uint8_t destination_id;  // target receiver id (not a MAC filter)
    uint8_t priority;        // preemption priority (higher preempts lower)
    uint8_t reserved;        // reserved: transmitters remain dumb, receivers map presets locally
    uint32_t seq_num;       // sequence number for diagnostics / dedup
};

static_assert(sizeof(Trigger) == 8, "Trigger must be 8 bytes");
