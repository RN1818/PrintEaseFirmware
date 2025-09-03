#include "print_settings.h"
#include "system_variables.h"
#include "display_handler.h"
#include "websocket_handler.h"
#include "file_handler.h"

enum ProcessingPhase {
    NotStarted,
    Running,
    Confirm,
    Done,
    Discard
};

char keys[ROWS][COLS] = {
    {'1','4','7','C'},
    {'2','5','8','0'},
    {'3','6','9','E'},
    {'S','M','L','D'},
    {'O','R','N','P'},
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
byte rowPins[ROWS] = {15, 39, 36, 35, 34};
byte colPins[COLS] = {16, 17, 22, 23};

static ProcessingPhase processingPhase = NotStarted;
static bool processingComplete = false;
bool isScreenUpdated = false;
bool isStageCompleted = true;
bool isPrintSettingsShown = false;
static int inputPhase = 0;

bool getPaperSize(char key);
bool getOrientation(char key);
bool getMode(char key);
bool getPageRange(char key);
bool getColor(char key);
bool getCopies(char key);
bool getConfirmation(char key);
void getPrintSettings(char key);
void getPageNumber(int stage, int value, int selectedPageBuffer[2], bool *isFirstDigit);
int getPaperSizeIndex(const char *target);


void handleProcessing(char key)
{
    switch (processingPhase)
    {
        case NotStarted:
            if (key == 'E')
            {
                processingPhase = Running;
                inputPhase = 0;
                return;
            }
            else if (key == 'P')
            {
                processingPhase = Confirm;
            }
            else if (key == 'D')
            {
                processingPhase = Discard;
            }
            break;

        case Running:
            if (processingComplete)
            {
                processingPhase = Confirm;
                Serial.println("Processing completed.");
                break;
            }
            getPrintSettings(key);
            break;
        
        case Confirm:
            if (getConfirmation(key))
            {
                processingPhase = Done;
                Serial.println("Confirmed print settings.");
                break;
            }
            break;

        case Done:
            processingPhase = NotStarted;
            processingComplete = false;
            isPrintSettingsShown = false;
            Serial.println("Printing...");
            currentState = STATE_SEND_SETTINGS;
            break;

        case Discard:
            processingPhase = NotStarted;
            processingComplete = false;
            isPrintSettingsShown = false;
            sendDiscard();
            Serial.println("Discarded print settings.");
            showSplashScreen("Printing canceled by user.");
            if (reaminingFilesGlobal > 0)
            {
                currentState = STATE_WAIT_FOR_FILE;
            }
            else
            {
                currentState = STATE_SPLASH;
            }
            break;
    }
}


bool getConfirmation(char key)
{   
    if (!isPrintSettingsShown)
    {   
        calculatePrice();
        showPrintSettings();
        Serial.println(printSettings.copies);
        isPrintSettingsShown = true;
        isStageCompleted = true;
    }
    switch (key)
    {
        case 'E':
            return true;
        case 'D':
            processingPhase = Discard;
            break;
        case 'S':
            inputPhase = 0;
            processingPhase = Running;
            processingComplete = false;
            isPrintSettingsShown = false;
            break;
        case 'O':
            inputPhase = 1;
            processingPhase = Running;
            processingComplete = false;
            isPrintSettingsShown = false;
            break;
        case 'M':
            inputPhase = 2;
            processingPhase = Running;
            processingComplete = false;
            isPrintSettingsShown = false;
            break;
        case 'R':
            inputPhase = 3;
            processingPhase = Running;
            processingComplete = false;
            isPrintSettingsShown = false;
            break;
        case 'L':
            inputPhase = 4;
            processingPhase = Running;
            processingComplete = false;
            isPrintSettingsShown = false;
            break;
        case 'N':
            inputPhase = 5;
            processingPhase = Running;
            processingComplete = false;
            isPrintSettingsShown = false;
            break;
    }
    return false;
}


void getPrintSettings(char key)
{
    switch (key)
    {
        case 'P':
            if (!isStageCompleted)
            {
                showError("Current stage is not completed");
                return;
            }
            processingComplete = true;
            return;
        case 'E':
            if (isStageCompleted)
            {
                inputPhase++;
                isScreenUpdated = false;
                return;
            }
        case 'S':
            if (isStageCompleted)
            {
                isScreenUpdated = false;
                inputPhase = 0;
                return;
            }
        case 'O':
            if (isStageCompleted)
            {
                isScreenUpdated = false;
                inputPhase = 1;
                return;
            }
        case 'M':
            if (isStageCompleted)
            {
                isScreenUpdated = false;
                inputPhase = 2;
                return;
            }
        case 'R':
            if (isStageCompleted)
            {
                isScreenUpdated = false;
                inputPhase = 3;
                return;
            }
        case 'L':
            if (isStageCompleted)
            {
                isScreenUpdated = false;
                inputPhase = 4;
                return;
            }
        case 'N':
            if (isStageCompleted)
            {
                isScreenUpdated = false;
                inputPhase = 5;
                return;
            }
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
            break;
        case 6:
            processingComplete = true;
    }
}


bool getPaperSize(char key)
{   
    static TFTMessage message;
    if (!isScreenUpdated)
    {   
        message.title = "Select the paper size:";
        for (int i = 0; i < 5; i++)
        {
            message.selections[i] = availablePaperSizes[i];
        }
        updateDisplay(message);
        highlight(message, getPaperSizeIndex(printSettings.paperSize.c_str()));
        isScreenUpdated = true;
    }
    if (key)
    {
        int value = key - '0';
        if (value < 1 || value > 5)
        {
            showError("Invalid Selection");
            return false;
        }
        highlight(message, value - 1);
        printSettings.paperSize = String(availablePaperSizes[value - 1]);
        return true;
    }
    return false;
}


bool getOrientation(char key)
{
    static TFTMessage message;
    if (!isScreenUpdated)
    {   
        message.title = "Select the orientation:";
        for (int i = 0; i < 2; i++)
        {
            message.selections[i] = availableOrientations[i];
        }
        updateDisplay(message);
        highlight(message, printSettings.orientation);
        isScreenUpdated = true;
    }
    if (key)
    {
        int value = key - '0';
        if (value < 1 || value > 2)
        {
            showError("Invalid Selection");
            return false;
        }
        highlight(message, value - 1);
        printSettings.orientation = value - 1;
        return true;
    }
    return false;
}


bool getMode(char key)
{
    static TFTMessage message;
    if (!isScreenUpdated)
    {   
        message.title = "Select the mode:";
        for (int i = 0; i < 2; i++)
        {
            message.selections[i] = availableModes[i];
        }
        updateDisplay(message);
        highlight(message, printSettings.mode);
        isScreenUpdated = true;
    }
    if (key)
    {
        int value = key - '0';
        if (value < 1 || value > 2)
        {
            showError("Invalid Selection");
            return false;
        }
        highlight(message, value - 1);
        printSettings.mode = value - 1;
        return true;
    }
    return false;
}


bool getPageRange(char key)
{
    static TFTMessage message;
    static int selectedPageBuffer[2] = {1, 1};
    static bool manuallySetRange = false;
    static int stage = -1;
    static bool isPageRangeOptionsShown = false;
    static bool isFirstDigit = true;

    if (!isScreenUpdated)
    {   
        message.title = "Select the page range:";
        for (int i = 0; i < 2; i++)
        {
            message.selections[i] = availablePageRanges[i];
        }
        updateDisplay(message);
        if (printSettings.range == "All")
        {
            highlight(message, 0);
        }
        else
        {
            highlight(message, 1);
        }
        isScreenUpdated = true;
    }
    if (key)
    {
        int value = key - '0';
        if (manuallySetRange)
        {
            if (key == 'E')
            {
                if (!isPageRangeOptionsShown)
                {
                    stage = 0;
                    showPageRangeOptions(stage, selectedPageBuffer[0], selectedPageBuffer[1]);
                    isPageRangeOptionsShown = true;
                    return false;
                }
            }
            if (key == 'C')
            {
                if (isPageRangeOptionsShown)
                {   
                    selectedPageBuffer[stage] = 1;
                    isFirstDigit = true;
                    getPageRangeOptions(stage, selectedPageBuffer[0], selectedPageBuffer[1]);
                }
                return false;
            }
            switch (stage)
            {
                case 0:
                    if (key == 'E')
                    {
                        stage++;
                        showPageRangeOptions(stage, selectedPageBuffer[0], selectedPageBuffer[1]);
                        isFirstDigit = true;
                        return false;
                    }
                    getPageNumber(0, value, selectedPageBuffer, &isFirstDigit);
                    break;

                case 1:
                    if (key == 'E')
                    {
                        if (selectedPageBuffer[1] < selectedPageBuffer[0])
                        {
                            showError("End page is before start page");
                            selectedPageBuffer[1] = 1;
                            isFirstDigit = true;
                            getPageRangeOptions(stage, selectedPageBuffer[0], selectedPageBuffer[1]);
                            return false;
                        }
                        printSettings.range = String(selectedPageBuffer[0]) + "-" + String(selectedPageBuffer[1]);
                        selectedPageBuffer[0] = 1;
                        selectedPageBuffer[1] = 1;
                        stage = -1;
                        manuallySetRange = false;
                        isPageRangeOptionsShown = false;
                        isStageCompleted = true;
                        isScreenUpdated = false;
                        isFirstDigit = true;
                        inputPhase++;
                        return true;
                    }
                    getPageNumber(1, value, selectedPageBuffer, &isFirstDigit);
                    break;
            }
        }
        else
        {
            if (value < 1 || value > 2)
            {
                showError("Invalid Selection");
                return false;
            }
            highlight(message, value - 1);

            if (value == 2)
            {
                manuallySetRange = true;
                isStageCompleted = false;
            }
            else
            {
                printSettings.range = "All";
                return true;
            }
        }
    }
    return false;
}


bool getColor(char key)
{
    static TFTMessage message;
    if (!isScreenUpdated)
    {   
        message.title = "Select color mode:";
        for (int i = 0; i < 2; i++)
        {
            message.selections[i] = availableColors[i];
        }
        updateDisplay(message);
        highlight(message, printSettings.color);
        isScreenUpdated = true;
    }
    if (key)
    {
        int value = key - '0';
        if (value < 1 || value > 2)
        {
            showError("Invalid Selection");
            return false;
        }
        highlight(message, value - 1);
        printSettings.color = value - 1;
        Serial.println("Color: " + String(availableColors[printSettings.color]));
        return true;
    }
    return false;
}


bool getCopies(char key)
{
    static TFTMessage message;
    static int copies = printSettings.copies;
    static int isFirstDigit = true;
    isStageCompleted = false;

    if (!isScreenUpdated)
    {   
        message.title = "Number of copies:";
        updateDisplay(message);
        showCopies(copies);
        isScreenUpdated = true;
    }
    if (key)
    {
        int value = key - '0';
        if (key == 'E')
        {
            if (copies == 0)
            {
                showError("Cannot print 0 copies");
                copies = 1;
                showCopies(copies);
                return false;
            }
            isFirstDigit = true;
            isStageCompleted = true;
            inputPhase++;
            isScreenUpdated = false;
            return true;
        } 
        else if (key == 'C')
        {
            copies = 1;
            isFirstDigit = true;
            showCopies(copies);
        }
        else
        {
            if (value < 0 || value > 9)
            {
                showError("Invalid Selection");
                return false;
            }
            if (isFirstDigit)
            {
                if (value == 0)
                {
                    showError("The first digit cannot be 0");
                    return false;
                }
                copies = value;
                isFirstDigit = false;
                showCopies(copies);
                printSettings.copies = copies;
                return false;
            }
            copies = copies * 10 + value;
            if (copies > MAX_NUM_COPIES)
            {
                showError("Maximum number of copies is 1000");
                copies = 1;
                showCopies(copies);
                isFirstDigit = true;
                return false;
            }
            printSettings.copies = copies;
            showCopies(copies);
        }
    }
    return false;
}


void getPageNumber(int stage, int value, int selectedPageBuffer[2], bool *isFirstDigit)
{
    if ((value < 0 || value > 9) && value != 'E')
    {
        showError("Invalid page number");
        return;
    }
    else
    {
        if (*isFirstDigit)
        {
            if (value == 0)
            {
                showError("The first digit cannot be 0");
                return;
            }
            if (value > numberOfPagesGlobal)
            {
                showError("Page number out of range");
                return;
            }
            selectedPageBuffer[stage] = value;
            *isFirstDigit = false;
            getPageRangeOptions(stage, selectedPageBuffer[0], selectedPageBuffer[1]);
            return;
        }
        selectedPageBuffer[stage] = selectedPageBuffer[stage] * 10 + value;
        if (selectedPageBuffer[stage] > numberOfPagesGlobal)
        {
            showError("Page number out of range");
            selectedPageBuffer[stage] = 1;
            *isFirstDigit = true;
        }
        getPageRangeOptions(stage, selectedPageBuffer[0], selectedPageBuffer[1]);
    }
    return;
}


int getPaperSizeIndex(const char *target)
{
    for (int i = 0; i < 5; ++i) {
        if (strcmp(availablePaperSizes[i], target) == 0) {
            return i;
        }
    }
    return -1;
}