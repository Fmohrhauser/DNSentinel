#ifndef BLOCKLIST_H
#define BLOCKLIST_H


#include <Arduino.h>
#include <set>

extern std::set<String> blockedDomains;
struct ImportResult
{
    int added = 0;
    int duplicates = 0;
    int ignored = 0;
};

void loadBlocklist();
void saveBlocklist();

bool addBlockedDomain(String domain);
bool removeBlockedDomain(String domain);

String createBlocklistJSON();

bool isBlocked(String domain);

ImportResult importBlocklist(String data);

#endif