#include "dashboard.h"
#include <WebServer.h>
#include "stats.h"
#include "query_log.h"
#include <LittleFS.h>
#include "system.h"
#include "blocked_stats.h"

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

    server.on("/script.js", HTTP_GET,[](){
        File file = LittleFS.open("/script.js", "r");

        if(!file)
        {
            server.send(404, "text/plain", "File not found");

            return;
        }

        server.streamFile(file, "application/javascript");

        file.close();
    });

    server.on("/style.css", HTTP_GET, [](){
        File file = LittleFS.open("/style.css", "r");

        if(!file)
        {
            server.send(404, "text/plain", "File not found");

            return;
        }

        server.streamFile(file, "text/css");

        file.close();
    });

    server.on("/api/system", HTTP_GET, [](){
        
        server.send(
            200,
            "application/json",
            createSystemJSON()
        );
    });

    server.on("/settings.svg", HTTP_GET, [](){

        File file = LittleFS.open("/settings.svg", "r");

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
            "image/svg+xml"
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