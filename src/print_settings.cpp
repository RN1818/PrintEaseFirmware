#include "print_settings.h"
#include "system_variables.h"
#include "display_handler.h"

enum ProcessingPhase {
    NotStarted,
    Running,
    Done
};

struct Settings {
    String paperSize = "A4";
    String orientation = "Portrait";
    String mode = "Duplex";
    String range = "All";
    String color = "Color";
    int copies = 1;
};

char keys[ROWS][COLS] = {
    {'1','2','3','S'},
    {'4','5','6','O'},
    {'7','8','9','M'},
    {'C','0','E','P'},
    {'R','C','N','D'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
byte rowPins[ROWS] = {39, 36, 34, 35, 23};
byte colPins[COLS] = {16, 17, 19, 22};
static ProcessingPhase processingPhase = NotStarted;
static bool processingComplete = false;
bool screenUpdated = false;
Settings printSettings;
String availablePaperSizes[5] = {"A4", "A3", "A5", "Letter", "Legal"};
static int inputPhase = 0;

bool getPaperSize(char key);
bool getOrientation(char key);
bool getMode(char key);
bool getPageRange(char key);
bool getColor(char key);
bool getCopies(char key);
void getPrintSettings(char key);


void handleProcessing(char key)
{
    switch (processingPhase)
    {
        case NotStarted:
            if (key == 'E')
            {
                processingPhase = Running;
                inputPhase = 0;
                Serial.println("Processing phase started.");
            }
            break;

        case Running:
            if (processingComplete) {
                processingPhase = Done;
                Serial.println("Processing completed.");
                break;
            }
            getPrintSettings(key);
            break;

        case Done:
            currentState = STATE_CONFIRM_PRINT;
            processingPhase = NotStarted;
            processingComplete = false;
            break;
    }
}

void getPrintSettings(char key)
{
    bool settingsUpdated = false;
    switch (key)
    {
        case 'P':
            processingComplete = true;
            return;
        case 'E':
            inputPhase++;
            return;
    }

    switch (inputPhase)
    {
        case 0:
            getPaperSize(key);
            break;
        case 1:
            getOrientation(key);
            break;
        case 2:
            getMode(key);
            break;
        case 3:
            getPageRange(key);
            break;
        case 4:
            getColor(key);
            break;
        case 5:
            getCopies(key);
    }
    if (settingsUpdated)
    {
        inputPhase++;
    }
}

bool getPaperSize(char key)
{
    static TFTMessage message_size;
    if (!screenUpdated)
    {   
        message_size.title = "Select the paper size:";
        for (int i = 0; i < 5; i++)
        {
            message_size.selections[i] = availablePaperSizes[i].c_str();
        }
        updateDisplay(message_size);
        screenUpdated = true;
    }
    if (key)
    {
        int value = key - '0';
        if (value < 1 || value > 5)
        {
            showError("Invalid Selection");
            return false;
        }
        highlight(message_size, value - 1);
        printSettings.paperSize = availablePaperSizes[value - 1];
        return true;
    }
    return false;
}

bool getOrientation(char key)
{
    return false;
}

bool getMode(char key)
{
    return false;
}

bool getPageRange(char key)
{
    return false;
}

bool getColor(char key)
{
    return false;
}

bool getCopies(char key)
{
    return false;
}