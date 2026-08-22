#include "dns_health.h"
#include "dns_server.h"
#include <ArduinoJson.h>
#include <Arduino.h>
#include "dns_health_stats.h"

String createDNSHealthJSON()
{
    String json = "{";

    json += "\"checked\":";
    json += upstreamChecked ? "true" : "false";
    json += ",";

    json += "\"online\":";
    json += upstreamOnline ? "true" : "false";
    json += ",";

    json += "\"requests\":";
    json += upstreamRequests;
    json += ",";

    json += "\"failures\":";
    json += upstreamFailures;
    json += ",";

    float successRate = 0;

    if(upstreamRequests > 0)
    {
        successRate =
            ((float)(upstreamRequests - upstreamFailures)
            / upstreamRequests)
            *100.0;
    }

    json += "\"successRate\":";
    json += String(successRate,1);
    json += ",";

    float averageLatency = 0;

    if(upstreamRequests > 0)
    {
        averageLatency =
            (float)totalUpstreamLatency /
            upstreamRequests;
    }

    json += "\"averageLatency\":";
    json += String(averageLatency,1);
    json += ",";

    json += "\"lastSuccess\":";
    json += lastUpstreamSuccess;

    json += "}";

    return json;

}