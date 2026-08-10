#ifndef WHITELIST_H
#define WHITELIST_H

#include <Arduino.h>
#include "hash_table.h"

extern DomainHashTable whitelistedDomains;

void loadWhiteList();
void saveWhiteList();

bool addWhitelistedDomain();
bool removeWhitelistedDomain();

bool isWhitelisted(String domain);

#endif