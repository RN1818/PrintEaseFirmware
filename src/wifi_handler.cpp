#include "wifi_handler.h"

const char* ssid = "ESP32";

void initWiFi()
{
    WiFi.softAPConfig(
        IPAddress(192, 168, 4, 1),
        IPAddress(192, 168, 4, 1),
        IPAddress(255, 255, 255, 0)
      );
    
    WiFi.softAP(ssid, NULL);
    Serial.printf("AP Started. IP: %s\n", WiFi.softAPIP().toString().c_str());
    
    WiFi.setSleep(false);
}