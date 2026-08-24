#pragma once

#include <Arduino.h>

struct CacheStatistics
{
    unsigned long hits;
};

extern CacheStatistics cacheStats;

void initCacheStats();

void recordCacheHit(unsigned long lookupTime);

void recordCacheMiss(unsigned long lookupTime);

