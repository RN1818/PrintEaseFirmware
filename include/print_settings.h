#pragma once

#include <Keypad.h>

#define MAX_NUM_COPIES 1000

constexpr byte ROWS = 5;
constexpr byte COLS = 4;
extern char keys[ROWS][COLS];
extern byte rowPins[ROWS];
extern byte colPins[COLS];
extern Keypad keypad;

void handleProcessing(char key);