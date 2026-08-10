#include "whitelist.h"
#include <LittleFS.h>
#include "domain_utils.h"
#include <ArduinoJson.h>

DomainHashTable whitelistedDomains;

void loadWhitelist()
{
    File file = LittleFS.open("/whitelist.txt", "r");

    if(!file)
    {
        Serial.println("Couldn't open whitelist.");
        return;
    }

    while(file.available())
    {
        String domain = file.readStringUntil('\n');

        domain = normalizeDomain(domain);
        if(domain.length() > 0)
        {
            whitelistedDomains.add(domain);
        }
    }

    file.close();

    Serial.print("Loaded ");
    Serial.print(whitelistedDomains.size());
    Serial.println(" whitelisted domains.");
}

void saveWhitelist()
{
    File file = LittleFS.open("/whitelist.txt", "w");

    if(!file)
    {
        Serial.println("couldn't save whitelist.");
        return;
    }

    for(int i = 0; i < whitelistedDomains.size(); i++)
    {
        file.println(whitelistedDomains.get(i));
    }
    file.close();

    Serial.println("Whitelist saved.");
}

bool addWhitelistedDomain(String domain)
{   
    domain = normalizeDomain(domain);

    if(domain.length() == 0)
    {
        return false;
    }

    if(!whitelistedDomains.add(domain))
    {
        return false;
    }

    saveWhitelist();

    return true;
}

bool removeWhitelistedDomain(String domain)
{   
    domain = normalizeDomain(domain);

    auto result = whitelistedDomains.remove(domain);

    if(result)
    {
        saveWhitelist();
        return true;
    }

    return false;
}


String createWhitelistJSON()
{
    JsonDocument doc;

    JsonArray array = doc.to<JsonArray>();

    for(int i = 0; i < whitelistedDomains.size(); i++)
    {
        array.add(whitelistedDomains.get(i));
    }

    String json;

    serializeJson(doc, json);

    return json;
}



bool isWhitelisted(String domain)
{
    domain = normalizeDomain(domain);

    return whitelistedDomains.contains(domain);
}

int getWhitelistSize()
{
    return whitelistedDomains.size();
}

void clearWhitelist()
{
    whitelistedDomains.clear();

    saveWhitelist();
}