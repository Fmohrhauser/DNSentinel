#include "cache_stats.h"

CacheStatistics cacheStats;

void initCacheStats()
{
    cacheStats.totalLookups = 0;

    cacheStats.hits = 0;

    cacheStats.misses = 0;

    cacheStats.totalLookupTime = 0;

    cacheStats.longestLookup = 0;

    cacheStats.shortestLookup = 0xFFFFFFFF;
}

void recordCacheHit(unsigned long lookupTime)
{
    cacheStats.totalLookups++;

    cacheStats.hits++;

    cacheStats.totalLookupTime += lookupTime;

    if(lookupTime > cacheStats.longestLookup)
    {
        cacheStats.longestLookup = lookupTime;
    }

    if(lookupTime < cacheStats.shortestLookup)
    {
        cacheStats.shortestLookup = lookupTime;
    }
}

void recordCacheMiss(unsigned long lookupTime)
{
    cacheStats.totalLookups++;

    cacheStats.misses++;

    cacheStats.totalLookupTime += lookupTime;

    if(lookupTime > cacheStats.longestLookup)
    {
        cacheStats.longestLookup = lookupTime;
    }

    if(lookupTime < cacheStats.shortestLookup)
    {
        cacheStats.shortestLookup = lookupTime;
    }
}