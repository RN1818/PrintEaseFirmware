#pragma once

#include <Arduino.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <cstring>

void webSocketEvent(uint8_t clientNum, WStype_t type, uint8_t * payload, size_t length);
void initWebSocket();
void handleWebSocket();