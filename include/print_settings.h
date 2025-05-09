#pragma once

#include <Keypad.h>

constexpr byte ROWS = 5;
constexpr byte COLS = 4;
extern char keys[ROWS][COLS];
extern byte rowPins[ROWS];
extern byte colPins[COLS];
extern Keypad keypad;

void handleProcessing(char key);