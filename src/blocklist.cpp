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

        if(count % 5000 == 0)
        {
          DEBUG_PRINT("Loaded so far: ");
          DEBUG_PRINTLN(count);

          DEBUG_PRINT("Free PSRAM: ");
          DEBUG_PRINTLN(ESP.getFreePsram());

          DEBUG_PRINT("Blocklist pool used: ");
          DEBUG_PRINTLN(blockedDomains.getPoolUsed());

          DEBUG_PRINT("Blocklist pool capacity: ");
          DEBUG_PRINTLN(blockedDomains.getPoolCapacity());
        }
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

  for(size_t i = 0; i < blockedDomains.getSlotCount(); i++)
  {
    String domain =
      blockedDomains.getSlot(i);

    if(domain.length() > 0)
    {
      file.println(domain);
    }
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

  for(size_t i = 0; i < blockedDomains.getSlotCount(); i++)
  {
    String domain =
      blockedDomains.getSlot(i);

    if(domain.length() > 0)
    {
      array.add(domain);
    }
  }

  String json;

  serializeJson(doc, json);

  return json;
}

String createBlocklistPageJSON(
  int offset,
  int limit,
  String search
)
{
  JsonDocument doc;

  JsonArray domains =
    doc["domains"].to<JsonArray>();

  search.toLowerCase();
  
  int matched = 0;
  int added = 0;

  for(size_t i = 0; i < blockedDomains.getSlotCount(); i++)
  {
    String domain =
      blockedDomains.getSlot(i);

    if(domain.length() == 0)
    {
      continue;
    }

    domain.toLowerCase();

    if(search.length() > 0)
    {
      if(domain.indexOf(search) == -1)
      {
        continue;
      }
    }

    if(matched >= offset &&
        added < limit)
      {
        domains.add(domain);
        added++;
      }

      matched++;
  }

  doc["offset"] = offset;
  doc["limit"] = limit;
  doc["total"] = matched;

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
    else if(!validDomain(domain))
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