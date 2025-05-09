#include <atomic>
#include "display_handler.h"
#include "wifi_handler.h"
#include "websocket_handler.h"
#include "system_variables.h"
#include "print_settings.h"

#define X_OFFSET 10
#define Y_OFFSET 10

TFTMessage message;
bool processingInitiated = false;
bool processingComplete = false;

void setup()
{
    Serial.begin(115200);
    initDisplay();
    initWiFi();
    initWebSocket();

    message.title = "Select paper size:";
    message.selections[0] = "A4";
    message.selections[1] = "A5";
    message.selections[2] = "A3";
    message.selections[3] = "Letter";
    message.selections[4] = "Legal";
}

void loop() 
{
    handleWebSocket();
    char key = keypad.getKey();

    switch (currentState)
    {
        case STATE_SPLASH:
            showSplashScreen();
            delay(2000);
            currentState = STATE_WAIT_FOR_URL;
            break;

        case STATE_PROCESSING:
            handleProcessing(key);

    }
}
    