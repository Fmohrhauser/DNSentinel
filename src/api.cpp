#include "api.h"
#include "stats.h"
#include "dashboard.h"
#include "query_log.h"

#include <WebServer.h>

extern WebServer server;

void startAPI()
{

    server.on("/api/stats", HTTP_GET, []()
    {
        server.send(
            200,
            "application/json",
            createStatsJSON()
        );
    });

    server.on("/api/logs", HTTP_GET, []()
    {
        server.send(
            200,
            "application/json",
            createQueryLogJSON()
        );
    });

}