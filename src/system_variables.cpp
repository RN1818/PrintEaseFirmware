#include "system_variables.h"

const char *ID = "printeasev1.0";
std::atomic<SystemState> currentState{STATE_SPLASH};
int numberOfPagesGlobal = 1;
Settings printSettings;

String availablePaperSizes[5] = {"A4", "A3", "A5", "Letter", "Legal"};
String availableOrientations[2] = {"Portrait", "Landscape"};
String availableModes[2] = {"Double-sided", "Single-sided"};
String availablePageRanges[2] = {"All", "Set page range"};
String availableColors[2] = {"Black and White", "Full Color"};