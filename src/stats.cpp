#include "stats.h"
#include "debug.h"

unsigned long totalRequests = 0;
unsigned long blockedRequests = 0;
unsigned long forwardedRequests = 0;
int cacheHits = 0;

unsigned long minuteStart = 0;
unsigned long requestsThisMinute = 0;

unsigned long lastMinuteRequests = 0;

unsigned long peakQueriesPerMinute = 0;

void initStats()
{
    totalRequests = 0;
    blockedRequests = 0;
    forwardedRequests = 0;

    cacheHits = 0;

    minuteStart = millis();

    requestsThisMinute = 0;

    lastMinuteRequests = 0;
}

void resetStats()
{
    initStats();

    DEBUG_PRINTLN("Statistics reset");
}


void incrementCacheHits()
{
    cacheHits++;
}

int getCacheHits()
{
    return cacheHits;
}

void incrementTotalRequests()
{
  
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

float getBlockedPercent()
{
    if(totalRequests == 0)
        return 0;
    
    return  (blockedRequests * 100.0f) /
        totalRequests;
}

float getForwardedPercent()
{
    if(totalRequests == 0)
        return 0;
    
    return
        (forwardedRequests * 100.0f) /
        totalRequests;
}

float getCachePercent()
{
    if(totalRequests == 0)
        return 0;

    return
        (cacheHits * 100.0f) /
        totalRequests;
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

float getAverageQueriesPerSecond()
{
    unsigned long uptime =
        millis() / 1000;

    if(uptime == 0)
        return 0;

    return
        (float)totalRequests /
        uptime;
}

float getSuccessRate()
{
    if(totalRequests == 0)
        return 100;

    return  
        (forwardedRequests * 100.0f) /
        totalRequests;
}



unsigned long getPeakQueriesPerMinute()
{
    return peakQueriesPerMinute;
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
    json += getCacheHits();
    json += ",";

    json += "\"blocked_percent\":";
    json += String(getBlockedPercent(),1);
    json += ",";

    json += "\"forwarded_percent\":";
    json += String(getForwardedPercent(),1);
    json += ",";

    json += "\"cache_percent\":";
    json += String(getCachePercent(),1);
    json += ",";

    json += "\"success_rate\":";
    json += String(getSuccessRate(),1);
    json += ",";

    json += "\"queries_per_minute\":";
    json += getQueriesPerMinute();
    json += ",";

    json += "\"peak_queries_per_minute\":";
    json += getPeakQueriesPerMinute();

    json += "}";

    return json;
}








