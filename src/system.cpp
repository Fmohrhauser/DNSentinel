#include "system.h"
#include <WiFi.h>

#include <Arduino.h>
#include <ArduinoJson.h>

String createSystemJSON()
{
    JsonDocument doc;

    doc["ip"] = WiFi.localIP().toString();
    doc["chip"] = ESP.getChipModel();
    doc["uptime"] = millis()/1000;
    doc["memory"] = ESP.getFreeHeap();

    String json;

    serializeJson(doc, json);

    return json;
}