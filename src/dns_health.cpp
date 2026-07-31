#include "dns_health.h"
#include "dns_server.h"
#include <ArduinoJson.h>
#include <Arduino.h>

String createDNSHealthJSON()
{
    JsonDocument doc;

    doc["online"] =
        upstreamOnline;

    doc["checked"] =
        upstreamChecked;

    doc["lastSuccess"] =
        lastUpstreamSuccess;

    if(upstreamRequests > 0)
    {
        doc["averageLatency"] =
            totalUpstreamLatency / upstreamRequests;
    }
    else
    {
        doc["averageLatency"] = 0;
    }

    doc["failures"] =
        upstreamFailures;

    String json;
    
    serializeJson(doc, json);

    return json;
}