#include "whitelist.h"
#include <LittleFS.h>

DomainHashTable whitelistedDomains;

void loadWhiteList()
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

        domain.trim();
        domain.toLowerCase();
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

void saveWhiteList()
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
    domain.toLowerCase();

    if(domain.length() == 0)
    {
        return false;
    }

    if(!whitelistedDomains.add(domain))
    {
        return false;
    }

    saveWhiteList();

    return true;
}

bool removeWhitelistedDomain(String domain)
{
    domain.toLowerCase();

    auto result = whitelistedDomains.remove(domain);

    if(result)
    {
        saveWhiteList();
        return true;
    }

    return false;
}



bool isWhitelisted(String domain)
{
    domain.trim();
    domain.toLowerCase();

    return whitelistedDomains.contains(domain);
}