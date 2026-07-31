#ifndef DNS_CACHE_H
#define DNS_CACHE_H

#include <Arduino.h>
#include "config.h"


struct CacheEntry
{
    bool valid;

    String domain;
    uint16_t qType;

    byte response[MAX_DNS_PACKET_SIZE];

    int responseLength;
    unsigned long expiresAt;
};


bool cacheLookup(
    const String& domain,
    uint16_t qType,
    byte response[],
    int& responseLength
);

void cacheInsert(
    const String& domain,
    uint16_t qType,
    const byte response[],
    int responseLength
);

void cacheCleanup();

void initCache();

#endif