#include "stats.h"

unsigned long totalRequests = 0;
unsigned long blockedRequests = 0;
unsigned long forwardedRequests = 0;

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

    json += "}";

    return json;
}