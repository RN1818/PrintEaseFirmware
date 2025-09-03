#include "system_variables.h"
#include "websocket_handler.h"
#include "print_settings.h"
#include "display_handler.h"
#include "file_handler.h"

const int wsPort = 81;
uint8_t client;

WebSocketsServer webSocket(wsPort);

void webSocketEvent(uint8_t clientNum, WStype_t type, uint8_t * payload, size_t length)
{
    switch (type)
    {
        case WStype_CONNECTED:
            Serial.printf("[%u] Connection Established\n", clientNum);
            if (currentState == STATE_WAIT_FOR_URL)
            {
                showSplashScreen("Connected to the server.");
            }
            break;

        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected\n", clientNum);
            break;
        
        case WStype_TEXT:
            String message = (char*)payload;
            DynamicJsonDocument doc(2048);
            DeserializationError error = deserializeJson(doc, message);
            Serial.println(message);
            if (error) {
                Serial.printf("Failed to parse JSON: %s\n", error.c_str());
                return;
            }

            if (doc.isNull() || !doc.is<JsonObject>()) {
                Serial.println("Invalid JSON");
                return;
            }

            const char *id = nullptr; 
            if (doc.containsKey("id") && doc["id"].is<const char*>())
            {
                id = doc["id"];
            }
            else
            {
                Serial.println("Invalid ID");
                return;
            }

            if (strcmp(id, ID) != 0)
            {
                Serial.printf("Invalid ID: %s\n", id);
                return;
            }

            const char* action = nullptr; 
            if (doc.containsKey("action") && doc["action"].is<const char*>())
            {
                action = doc["action"];
            }
            else
            {
                Serial.println("Error: Missing or invalid 'action' field");
                return;
            }

            if (strcmp(action, "RESET") == 0)
            {   
                currentState = STATE_SPLASH;
                reaminingFilesGlobal = 0;
                showSplashScreen("System restarted.");
                return;
            }
            else if (strcmp(action, "PRINT_RATES") == 0)
            {   
                storeRates(doc);
                return;
            }
            else if (strcmp(action, "FILE_UPLOAD_URL") == 0 && currentState == STATE_WAIT_FOR_URL)
            {   
                client = clientNum;
                currentState = STATE_WAIT_FOR_FILE;
                const char *url = doc["url"];
                showQR(url);
                return;
            }
            else if (strcmp(action, "INPUT_PRINT_SETTINGS") == 0 && currentState == STATE_WAIT_FOR_FILE)
            {   
                currentState = STATE_PROCESSING;
                processFile(doc);
            }
            else if (strcmp(action, "PRINTER_ID") == 0 && currentState == STATE_SEND_SETTINGS)
            {
                const char *id = doc["printer_id"];
                printerID = String(id);
            }
            else
            {
                Serial.printf("Invalid action: %s\n", action);
                return;
            }


    }
}


void initWebSocket()
{
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}


void handleWebSocket()
{
    webSocket.loop();
}


void sendTimeout()
{
    webSocket.sendTXT(client, "{\"status\":\"TIMEOUT\"}");
}


void sendDiscard()
{
    webSocket.sendTXT(client, "{\"status\":\"DISCARDED\"}");
}


void sendPrintSettings()
{
    DynamicJsonDocument doc(512);

    doc["status"] = "PRINT_SETTINGS";
    JsonObject settings = doc.createNestedObject("settings");

    settings["paper_size"] = printSettings.paperSize;
    settings["orientation"] = printSettings.orientation;
    settings["mode"] = printSettings.mode;
    settings["page_range"] = printSettings.range;
    settings["color"] = printSettings.color;
    settings["copies"] = printSettings.copies;
    settings["price"] = printSettings.price;
    settings["papers"] = printSettings.papers;

    String message;
    serializeJson(doc, message);
    webSocket.sendTXT(client, message);
    Serial.println(message);
}