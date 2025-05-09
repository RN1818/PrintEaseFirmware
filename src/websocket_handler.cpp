#include "system_variables.h"
#include "websocket_handler.h"
#include "print_settings.h"
#include "display_handler.h"
//#include "file_handler.h"

const int wsPort = 81;

WebSocketsServer webSocket(wsPort);

void webSocketEvent(uint8_t clientNum, WStype_t type, uint8_t * payload, size_t length)
{
    switch (type)
    {
        case WStype_CONNECTED:
            Serial.printf("[%u] Connection Established\n", clientNum);
            break;

        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected\n", clientNum);
            break;
        
        case WStype_TEXT:
            String message = (char*)payload;
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, message);
            Serial.println(message);
            if (error) {
                Serial.printf("Failed to parse JSON: %s\n", error.c_str());
                return;
            }

            const char* id = doc["id"];
            if (strcmp(id, ID) != 0)
            {
                Serial.printf("Invalid ID: %s\n", id);
                return;
            }

            const char* action = doc["action"];
            if (strcmp(action, "PRINT_RATES") == 0)
            {   
                webSocket.sendTXT(clientNum, "{\"ACK\":\"PRINT_RATES\"}");
                //getRates(doc);
                return;
            }
            else if (strcmp(action, "FILE_UPLOAD_URL") == 0 && currentState == STATE_WAIT_FOR_URL)
            {   
                currentState = STATE_WAIT_FOR_FILE;
                webSocket.sendTXT(clientNum, "{\"ACK\":\"FILE_UPLOAD_URL\"}");
                const char *url = doc["url"];
                showQR(url);
                return;
            }
            else if (strcmp(action, "INPUT_PRINT_SETTINGS") == 0 && currentState == STATE_WAIT_FOR_FILE)
            {   
                currentState = STATE_PROCESSING;
                webSocket.sendTXT(clientNum, "{\"ACK\":\"INPUT_PRINT_SETTINGS\"}");
                const char *fileName = doc["file_name"];
                const int remainingFiles = (int)doc["remaining_files"];
                const int numberOfPages = (int)doc["number_of_pages"];
                showFileInfo(fileName, numberOfPages);
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