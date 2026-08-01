#include "settings.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

Settings settings;


void initializeSettings()
{
    settings.upstreamDNS = "1.1.1.1";

    settings.blockingEnabled = true;

    settings.cacheEnabled = true;

    settings.queryLoggingEnabled = true;
    settings.blockingMode = REDIRECT;
    settings.redirectIP = "192.168.50.147";
}

Settings getSettings()
{
    return settings;
}

void updateSettings(const Settings& newSettings)
{
    settings = newSettings;

    saveSettings();
}

String createSettingsJSON()
{
    JsonDocument doc;

    doc["upstreamDNS"] =
        settings.upstreamDNS;

    doc["blockingEnabled"] =
        settings.blockingEnabled;

    doc["cacheEnabled"] =
        settings.cacheEnabled;

    doc["queryLoggingEnabled"] =
        settings.queryLoggingEnabled;

    String json;

    serializeJson(doc, json);

    return json;
}

void saveSettings()
{
    File file = LittleFS.open("/settings.json","w");

    if(!file)
    {
        Serial.println("failed to open settings file");
        return;
    }

    JsonDocument doc;


    doc["upstreamDNS"] = settings.upstreamDNS;

    doc["blockingEnabled"] = 
        settings.blockingEnabled;

    doc["cacheEnabled"] = 
        settings.cacheEnabled;

    doc["queryLoggingEnabled"] = 
        settings.queryLoggingEnabled;

    doc["redirectIP"] = settings.redirectIP;

    serializeJson(doc,file);

    file.close();

    Serial.println("Settings saved");
}

void loadSettings()
{
    if(!LittleFS.exists("/settings.json"))
    {
        Serial.println("No settings file found");

        saveSettings();

        return;
    }

    File file = LittleFS.open("/settings.json", "r");

    JsonDocument doc;

    DeserializationError error =
        deserializeJson(doc, file);

    if(error)
    {
        Serial.println("Failed loading settings");

        return;
    }
    if(doc["upstreamDNS"].is<String>())
    {
        settings.upstreamDNS =
            doc["upstreamDNS"].as<String>();
    }
    if(doc["blockingEnabled"].is<bool>())
    {
    settings.blockingEnabled =
        doc["blockingEnabled"];
    }
    if(doc["cacheEnabled"].is<bool>())
    {
    settings.cacheEnabled =
        doc["cacheEnabled"];
    }
    if(doc["queryLoggingEnabled"].is<bool>())
    {
    settings.queryLoggingEnabled =
        doc["queryLoggingEnabled"];
    }
    if(doc["redirectIP"].is<String>())
    {
        settings.redirectIP =
            doc["redirectIP"].as<String>();
    }
    file.close();

    Serial.println("Settings loaded");
}

