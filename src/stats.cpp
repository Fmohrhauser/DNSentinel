#include "stats.h"
#include "debug.h"
#include "cache_stats.h"

unsigned long totalRequests = 0;
unsigned long blockedRequests = 0;
unsigned long forwardedRequests = 0;

unsigned long minuteStart = 0;
unsigned long requestsThisMinute = 0;

unsigned long lastMinuteRequests = 0;

unsigned long peakQueriesPerMinute = 0;

void updateMinuteCounter();


void incrementTotalRequests()
{
    updateMinuteCounter();

    totalRequests++;
    requestsThisMinute++;
}

void incrementBlockedRequests()
{

    blockedRequests++;
}

void incrementForwardedRequests()
{
    forwardedRequests++;
}



void updateMinuteCounter()
{
    if(millis() - minuteStart >= 60000)
    {
        lastMinuteRequests =
            requestsThisMinute;

        if(lastMinuteRequests > peakQueriesPerMinute)
        {
            peakQueriesPerMinute =
                lastMinuteRequests;
        }

        requestsThisMinute = 0;

        minuteStart = millis();
    }
}

unsigned long getQueriesPerMinute()
{
    updateMinuteCounter();

    return lastMinuteRequests;
}


String createStatsJSON()
{
    String json ="{";

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
    json += cacheStats.hits;
    json += ",";

    json += "\"queries_per_minute\":";
    json += getQueriesPerMinute();

    json += "}";

    return json;
}








