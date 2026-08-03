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

float getCacheHitRate()
{
    if(cacheStats.totalLookups == 0)
    {
        return 0;
    }

    return
        (100.0f * cacheStats.hits)
        /
        cacheStats.totalLookups;
}

unsigned long getAverageLookupTime()
{
    if(cacheStats.totalLookups == 0)
    {
        return 0;
    }

    return 
        cacheStats.totalLookupTime
        /
        cacheStats.totalLookups;
}

String createCacheStatsJSON()
{
    String json = "{";

    json += "\"lookups\":";
    json += cacheStats.totalLookups;
    json += ",";

    json += "\"hits\":";
    json += cacheStats.hits;
    json += ",";

    json += "\"misses\":";
    json += cacheStats.misses;
    json += ",";

    json += "\"hitRate\":";
    json += String(getCacheHitRate(),2);
    json += ",";

    json += "\"averageLookup\":";
    json += getAverageLookupTime();
    json += ",";

    json += "\"longestLookup\":";
    json += cacheStats.longestLookup;
    json += ",";

    json += "\"shortestLookup\":";

    if(cacheStats.shortestLookup == 0xFFFFFFFF)
    {
        json += 0;
    }
    else
    {
        json += cacheStats.shortestLookup;
    }

    json += "}";

    return json;
}