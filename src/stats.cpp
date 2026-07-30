#include "stats.h"

unsigned long totalRequests = 0;
unsigned long blockedRequests = 0;
unsigned long forwardedRequests = 0;
int cacheHits = 0;

void incrementCacheHits()
{
    cacheHits++;
}

int getCacheHits()
{
    return cacheHits;
}

String createStatsJSON()
{
    String json = "{";

    json += "\"total\":";
    json += totalRequests;
    json += ",";

    json += "\"blocked\":";
    json += blockedRequests;
    json += ",";

    json += "\"forwarded\":";
    json += forwardedRequests;
    json += ",";

    json += "\"cache_hits\":";
    json += getCacheHits();

    json += "}";

    return json;
}

