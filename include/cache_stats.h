#pragma once

#include <Arduino.h>

struct CacheStatistics
{
    unsigned long totalLookups;

    unsigned long hits;

    unsigned long misses;

    unsigned long totalLookupTime;

    unsigned long longestLookup;

    unsigned long shortestLookup;
};

extern CacheStatistics cacheStats;

void initCacheStats();

void recordCacheHit(unsigned long lookupTime);

void recordCacheMiss(unsigned long lookupTime);

float getCacheHitRate();

unsigned long getAverageLookupTime();

String createCacheStatsJSON();
