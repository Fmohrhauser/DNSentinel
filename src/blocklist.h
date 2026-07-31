#ifndef BLOCKLIST_H
#define BLOCKLIST_H


#include <Arduino.h>
#include <vector>

extern std::vector<String> blockedDomains;

void loadBlocklist();
void saveBlocklist();

bool addBlockedDomain(String domain);
bool removeBlockedDomain(String domain);

String createBlocklistJSON();

bool isBlocked(String domain);

#endif