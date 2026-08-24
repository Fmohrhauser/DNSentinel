#include "cache_stats.h"

CacheStatistics cacheStats;

void initCacheStats()
{
    cacheStats.hits = 0;
}

void recordCacheHit(unsigned long lookupTime)
{
    cacheStats.hits++;
}