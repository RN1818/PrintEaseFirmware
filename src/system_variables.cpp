#include "system_variables.h"

const char *ID = "printeasev1.0";
std::atomic<SystemState> currentState{STATE_SPLASH};
int numberOfPagesGlobal = 1;
int reaminingFilesGlobal = 0;
Settings printSettings;
unsigned long MAX_WAIT_TIME = 1000 * 60 * 10;

const char *availablePaperSizes[5] = {"A4", "A3", "A5", "Letter", "Legal"};
const char *availableOrientations[2] = {"Portrait", "Landscape"};
const char *availableModes[2] = {"Double-sided", "Single-sided"};
const char *availablePageRanges[2] = {"All", "Set page range"};
const char *availableColors[2] = {"Black and White", "Full Color"};