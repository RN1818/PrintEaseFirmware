#pragma once

#include <ArduinoJson.h>

void storeRates(JsonDocument doc);
void processFile(JsonDocument doc);
void calculatePrice();