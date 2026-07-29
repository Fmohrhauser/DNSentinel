#include "dashboard.h"
#include <WebServer.h>
#include "stats.h"
#include "query_log.h"
#include <LittleFS.h>

WebServer server(80);

void startDashboard()
{
    server.on("/",HTTP_GET, [](){
        
        File file = LittleFS.open("/index.html","r");

        if(!file)
        {
            server.send(
                404,
                "text/plain",
                "File not found"
            );
            return;
        }

        server.streamFile(
        file,
        "text/html"
        );
        file.close();
    });

    server.begin();

    Serial.println("Dashboard started");

}

void handleDashboard()
{
    server.handleClient();
}