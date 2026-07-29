#include "system.h"
#include <WiFi.h>

#include <Arduino.h>

String createSystemJSON()
{
    String json = "{";

    json += "\"ip\":\"";
    json += WiFi.localIP().toString();
    json += "\",";

    json += "\"chip\":\"ESP32-S3\"";

    json += "}";

    return json;
}