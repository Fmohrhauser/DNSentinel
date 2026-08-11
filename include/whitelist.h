#ifndef WHITELIST_H
#define WHITELIST_H

#include <Arduino.h>
#include "hash_table.h"

extern DomainHashTable whitelistedDomains;
struct ImportResultWhitelist
{
    int added = 0;
    int duplicates = 0;
    int ignored = 0;
};

void loadWhitelist();
void saveWhitelist();

bool addWhitelistedDomain(String domain);
bool removeWhitelistedDomain(String domain);

String createWhitelistJSON();

bool isWhitelisted(String domain);

ImportResultWhitelist importWhitelist(String data);
int getWhitelistSize();
void clearWhitelist();

#endif