#ifndef WHITELIST_H
#define WHITELIST_H

#include <Arduino.h>
#include "hash_table.h"

extern DomainHashTable whitelistedDomains;

void loadWhiteList();
void saveWhiteList();

bool addWhitelistedDomain();
bool removeWhitelistedDomain();

String createWhitelistJSON();

bool isWhitelisted(String domain);
int getWhitelistSize();
void clearWhitelist();

#endif