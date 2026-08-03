#ifndef STATS_H
#define STATS_H

#include <Arduino.h>

struct DNSStatistics
{
    //Totals
    uint32_t totalQueries;
    uint32_t blockedQueries;
    uint32_t forwardedQueries;
    uint32_t cachehits;
    uint32_t cacheMisses;

    //Timing
    uint32_t fastestQuery;
    uint32_t slowestQuery;
    uint32_t averageLatency;

    uint64_t totalLatency;

    //blocklist
    uint32_t blocklistLookups;
    uint32_t blocklistHits;

    //cache
    uint32_t cacheLookup;
    uint32_t cacheStores;
    uint32_t cacheEvictions;

    //Errors
    uint32_t parseErrors;
    uint32_t upstreamFailures;
    uint32_t malformedPackets;

    //startup
    uint32_t bootTime;

    //peaks
    uint32_t peakQueriesPerMinute;
    uint32_t currentQueriesPerMinute;

    //runtime
    uint32_t uptimeSeconds;

};




int getCacheHits();
void incrementCacheHits();
String createStatsJSON();

void initStats();
void resetStats();

void incrementTotalRequests();
void incrementBlockedRequests();
void incrementForwardedRequests();
void incrementCacheHits();

float getBlockedPercent();
float getForwardedPercent();
float getCachePercent();

unsigned long getQueriesPerMinute();

float getAverageQueriesPerSecond();

float getSuccessRate();

float getCacheEfficiency();

float getBlockedRatio();

float getForwardedRatio();

#endif