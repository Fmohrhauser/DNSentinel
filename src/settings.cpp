#include "settings.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

Settings settings;

void initializeAuthentication(){
    settings.authEnabled = false;
    settings.username = "";
    settings.passwordHash ="";
}


void initializeSettings()
{
    settings.upstreamDNS = "1.1.1.1";

    settings.blockingEnabled = true;

    settings.cacheEnabled = true;

    settings.queryLoggingEnabled = true;
    settings.blockingMode = NULL_IP;
    settings.redirectIP = "0.0.0.0";
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

    doc["blockingMode"] =
        settings.blockingMode;

    doc["redirectIP"] =
        settings.redirectIP;

    doc["authEnabled"] =
        settings.authEnabled;

    doc["username"] =
        settings.username;

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

    doc["blockingMode"] =
        settings.blockingMode;

    doc["redirectIP"] = settings.redirectIP;


    doc["authEnabled"] =
        settings.authEnabled;

    doc["username"] =
        settings.username;

    doc["passwordHash"] =
        settings.passwordHash;



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
    if(doc["blockingMode"].is<int>())
    {
        settings.blockingMode =
            static_cast<BlockingMode>(
            doc["blockingMode"].as<int>()
            );
    }
    if(doc["redirectIP"].is<String>())
    {
        settings.redirectIP =
            doc["redirectIP"].as<String>();
    }
    if(doc["authEnabled"].is<bool>())
    {
        settings.authEnabled =
            doc["authEnabled"];
    }
    if(doc["username"].is<String>())
    {
        settings.username =
            doc["username"].as<String>();
    }
    if(doc["passwordHash"].is<String>())
    {
        settings.passwordHash =
            doc["passwordHash"].as<String>();
    }
    file.close();

    Serial.println("Settings loaded");
}
bool validIP(String ip)
{
  int start = 0;
  int parts = 0;

  for(int i =0; i <= ip.length(); i++)
  {
    if(ip[i] == '.' || i == ip.length())
    {
        String part = ip.substring(start, i);

        if(part.length() == 0)
        {
            return false;
        }

        for(int j = 0; j < part.length(); j++)
        {
            if(part[j] < '0' || part[j] > '9')
            {
                return false;
            }
        }

        int value = part.toInt();

        Serial.print("IP part: ");
        Serial.print(part);
        Serial.print(" value: ");
        Serial.println(value);

        if(value < 0 || value > 255)
        {
            return false;
        }

        parts++;
        start = i + 1;
    }
  }

  return parts == 4;
}

