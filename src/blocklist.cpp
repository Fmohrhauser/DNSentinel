#include "blocklist.h"
#include "debug.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "hash_table.h"
#include "domain_utils.h"

DomainHashTable blockedDomains;

void loadBlocklist()
{

  File file = LittleFS.open("/blocklist.txt", "r");

  if(!file)
  {
    DEBUG_PRINTLN("Couldn't open blocklist.");

    return;
  }

  int count = 0;

  while(file.available())
  {
    String domain = file.readStringUntil('\n');

    domain = normalizeDomain(domain);

    if(domain.length() > 0)
    {
      if(blockedDomains.add(domain))
      {
        count++;
      }
      else
      {
        DEBUG_PRINTLN("Hash table full or duplicate");
      }
    }
  }

  file.close();

  DEBUG_PRINT("Loaded ");
  DEBUG_PRINT(blockedDomains.size());
  DEBUG_PRINTLN(" blocked domains.");
  DEBUG_PRINT("Free heap: ");
  DEBUG_PRINTLN(ESP.getFreeHeap());
  DEBUG_PRINT("Free PSRAM: ");
  DEBUG_PRINTLN(ESP.getFreePsram());
}


void saveBlocklist()
{
  File file = LittleFS.open("/blocklist.txt", "w");

  if(!file)
  {
    DEBUG_PRINTLN("Couldn't save blocklist.");
    return;
  }

  for(int i = 0; i < blockedDomains.size(); i++)
{
  file.println(blockedDomains.get(i));
}

  file.close();
  DEBUG_PRINTLN("Blocklist saved.");
}


bool addBlockedDomain(String domain)
{
  domain.toLowerCase();

  domain = normalizeDomain(domain);

  if(domain.length() == 0)
  {
    return false;
  }

  if(!blockedDomains.add(domain))
  {
    return false;
  }

  saveBlocklist();

  return true;
}

bool removeBlockedDomain(String domain)
{

  domain.toLowerCase();

  auto result = blockedDomains.remove(domain);

  if(result)
  {
    saveBlocklist();
    return true;
  }

  return false;
}

String createBlocklistJSON()
{
  JsonDocument doc;

  JsonArray array = doc.to<JsonArray>();

  for(int i = 0; i < blockedDomains.size(); i++)
  {
    array.add(blockedDomains.get(i));
  }

  String json;

  serializeJson(doc, json);

  return json;
}

bool checkDomainAndParents(String domain)
{
  while(true)
  {
    if(blockedDomains.contains(domain))
    {
      return true;
    }

    int dot = domain.indexOf('.');

    if(dot == -1)
    {
      break;
    }

    domain = domain.substring(dot + 1);

    //Don't check TLDs
    if(domain.indexOf('.') == -1)
    {
      break;
    }
  }

  return false;
}



bool isBlocked(String domain)
{


  domain.toLowerCase();

  return checkDomainAndParents(domain);
}


ImportResultBlocklist importBlocklist(String data)
{

  ImportResultBlocklist result;

  int start = 0;

  while(start < data.length())
  {
    int end = data.indexOf('\n', start);

    if(end == -1)
    {
      end = data.length();
    }

    String domain = data.substring(start, end);

    start = end + 1;

    domain = normalizeDomain(domain);

    if(domain.length() == 0)
    {
      result.ignored++;
    }
    else
    {
      if(blockedDomains.add(domain))
      {
        result.added ++;
      }
      else
      {
        result.duplicates++;
      }
    }
  }

  saveBlocklist();

  return result;
}

int getBlocklistSize()
{
  return blockedDomains.size();
}
void clearBlocklist()
{
  blockedDomains.clear();

  saveBlocklist();
}