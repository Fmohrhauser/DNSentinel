#ifndef WHITELIST_H
#define WHITELIST_H

#include <Arduino.h>
#include "hash_table.h"

extern DomainHashTable whitelistedDomains;

void loadWhitelist();
void saveWhitelist();

bool addWhitelistedDomain(String domain);
bool removeWhitelistedDomain(String domain);

String createWhitelistJSON();

bool isWhitelisted(String domain);
int getWhitelistSize();
void clearWhitelist();

#endif