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
    json += ",";

    json += "\"uptime\":";
    json += String(millis() / 1000);
    json += ",";

    json += "\"memory\":";
    json += String(ESP.getFreeHeap());


    json += "}";

    return json;
}