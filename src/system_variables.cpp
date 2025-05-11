#include "system_variables.h"

const char *ID = "printeasev1.0";
std::atomic<SystemState> currentState{STATE_SPLASH};
int numberOfPagesGlobal = 1;