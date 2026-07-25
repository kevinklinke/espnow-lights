#pragma once

#include <stdint.h>

#if __has_include("ReceiverConfig.local.h")
#include "ReceiverConfig.local.h"
#endif

#ifndef RECEIVER_ID
#define RECEIVER_ID 1
#endif

static constexpr uint8_t kReceiverId = static_cast<uint8_t>(RECEIVER_ID);
