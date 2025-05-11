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
    int orientation = 0;
    int mode = 0;
    String range = "all";
    int color = 0;
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

String availablePaperSizes[5] = {"A4", "A3", "A5", "Letter", "Legal"};
String availableOrientations[2] = {"Portrait", "Landscape"};
String availableModes[2] = {"Double-sided", "Single-sided"};
String availablePageRanges[2] = {"All", "Set page range"};
String availableColors[2] = {"Black and White", "Full Color"};

static ProcessingPhase processingPhase = NotStarted;
static bool processingComplete = false;
bool isScreenUpdated = false;
bool isStageCompleted = true;
Settings printSettings;
static int inputPhase = 0;

bool getPaperSize(char key);
bool getOrientation(char key);
bool getMode(char key);
bool getPageRange(char key);
bool getColor(char key);
bool getCopies(char key);
void getPrintSettings(char key);
void getPageNumber(int stage, int value, int selectedPageBuffer[2]);


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
    static int selectedPageBuffer[2] = {0, 0};
    static bool manuallySetRange = false;
    static int stage = -1;
    static bool isPageRangeOptionsShown = false;

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
                    showPageRangeOptions();
                    isPageRangeOptionsShown = true;
                    stage = 0;
                    return false;
                }
            }  
            switch (stage)
            {
                case 0:
                    if (key == 'E')
                    {
                        if (selectedPageBuffer[0] == 0)
                        {
                            showError("Select a valid page number");
                            return false;
                        }
                        stage++;
                    }
                    getPageNumber(0, value, selectedPageBuffer);
                    break;

                case 1:
                    if (key == 'E')
                    {
                        if (selectedPageBuffer[1] == 0 && selectedPageBuffer[0] != 0)
                        {
                            showError("Select a valid page number");
                            return false;
                        }
                        if (selectedPageBuffer[1] < selectedPageBuffer[0])
                        {
                            showError("End page is before start page");
                            selectedPageBuffer[1] = 0;
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
                        inputPhase++;

                        return true;
                    }
                    getPageNumber(1, value, selectedPageBuffer);
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
                printSettings.range = "all";
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
    return false;
}


void getPageNumber(int stage, int value, int selectedPageBuffer[2])
{
    if ((value < 0 || value > 9) && value != 'E')
    {
        showError("Invalid page number");
        return;
    }
    else
    {
        selectedPageBuffer[stage] = selectedPageBuffer[stage] * 10 + value;
        Serial.println("Page Buffer " + String(stage) + ": " + String(selectedPageBuffer[stage]));
        Serial.println("Number of pages " + String(numberOfPagesGlobal));
        if (selectedPageBuffer[stage] > numberOfPagesGlobal)
        {
            showError("Page number out of range");
            selectedPageBuffer[stage] = 0;
        }
        getPageRangeOptions(stage, selectedPageBuffer[0], selectedPageBuffer[1]);
    }
    return;
}