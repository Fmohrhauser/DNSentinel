#include "api.h"
#include "stats.h"
#include "dashboard.h"

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

}