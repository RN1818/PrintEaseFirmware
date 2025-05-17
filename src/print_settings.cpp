#include "print_settings.h"
#include "system_variables.h"
#include "display_handler.h"

enum ProcessingPhase {
    NotStarted,
    Running,
    Confirm,
    Done,
    Discard
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
            else if (key == 'P')
            {
                processingPhase = Confirm;
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
            Serial.println("Printing...");
            currentState = STATE_SEND_SETTINGS;
            break;

        case Discard:
            processingPhase = NotStarted;
            processingComplete = false;
            currentState = STATE_SPLASH;
            Serial.println("Discarded print settings.");
            showSplashScreen("Printing canceled by user.");
            break;
    }
}


bool getConfirmation(char key)
{   
    if (!isPrintSettingsShown)
    {
        showPrintSettings();
        isPrintSettingsShown = true;
    }
    switch (key)
    {
        case 'E':
            return true;
        case 'C':
            processingPhase = Discard;
            return false;
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
                Serial.println(String("Input phase:") + inputPhase);
                isScreenUpdated = false;
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
            message.selections[i] = availablePaperSizes[i].c_str();
        }
        updateDisplay(message);
        highlight(message, 0);
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
        printSettings.paperSize = availablePaperSizes[value - 1];
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
            message.selections[i] = availableOrientations[i].c_str();
        }
        updateDisplay(message);
        highlight(message, 0);
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
        printSettings.orientation = value;
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
            message.selections[i] = availableModes[i].c_str();
        }
        updateDisplay(message);
        highlight(message, 0);
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
        printSettings.mode = value;
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
            message.selections[i] = availablePageRanges[i].c_str();
        }
        updateDisplay(message);
        highlight(message, 0);
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
                        printSettings.range = String(selectedPageBuffer[0] + "-" + selectedPageBuffer[1]);
                        selectedPageBuffer[0] = 0;
                        selectedPageBuffer[1] = 0;
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
            message.selections[i] = availableColors[i].c_str();
        }
        updateDisplay(message);
        highlight(message, 0);
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
        printSettings.mode = value;
        return true;
    }
    return false;
}


bool getCopies(char key)
{
    static TFTMessage message;
    static int copies = 1;
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
            copies = 1;
            isStageCompleted = true;
            inputPhase++;
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