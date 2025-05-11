#pragma once

#include <atomic>

enum SystemState {
    STATE_SPLASH,
    STATE_WAIT_FOR_URL,
    STATE_WAIT_FOR_FILE,
    STATE_PROCESSING,
    STATE_CONFIRM_PRINT,
    STATE_DONE
};

extern std::atomic <SystemState> currentState;
extern const char* ID;
extern int numberOfPagesGlobal;