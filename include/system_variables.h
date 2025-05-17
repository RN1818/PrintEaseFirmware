#pragma once

#include <atomic>
#include <Arduino.h>

enum SystemState {
    STATE_SPLASH,
    STATE_WAIT_FOR_URL,
    STATE_WAIT_FOR_FILE,
    STATE_PROCESSING,
    STATE_SEND_SETTINGS,
    STATE_DONE
};

struct Settings {
    String paperSize = "A4";
    int orientation = 0;
    int mode = 0;
    String range = "All";
    int color = 0;
    int copies = 1;
};

extern Settings printSettings;
extern std::atomic <SystemState> currentState;
extern const char* ID;
extern int numberOfPagesGlobal;

extern String availablePaperSizes[5];
extern String availableOrientations[2];
extern String availableModes[2];
extern String availablePageRanges[2];
extern String availableColors[2];