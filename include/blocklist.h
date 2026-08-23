#ifndef BLOCKLIST_H
#define BLOCKLIST_H


#include <Arduino.h>
#include "hash_table.h"


extern DomainHashTable blockedDomains;
struct ImportResultBlocklist
{
    int added = 0;
    int duplicates = 0;
    int ignored = 0;
};

void loadBlocklist();
void saveBlocklist();

bool addBlockedDomain(String domain);
bool removeBlockedDomain(String domain);

String createBlocklistPageJSON(
  int offset,
  int limit,
  String search);

bool isBlocked(String domain);

ImportResultBlocklist importBlocklist(String data);
int getBlocklistSize();
void clearBlocklist();

#endif