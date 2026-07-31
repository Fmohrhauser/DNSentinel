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

    server.on("/blocklist.html",HTTP_GET, [](){
        
        File file = LittleFS.open("/blocklist.html","r");

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

    server.on("/queries.html",HTTP_GET, [](){
        
        File file = LittleFS.open("/queries.html","r");

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

    server.on("/settings.html",HTTP_GET, [](){
        
        File file = LittleFS.open("/settings.html","r");

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

    server.on("/dashboard.js", HTTP_GET,[](){
        File file = LittleFS.open("/dashboard.js", "r");

        if(!file)
        {
            server.send(404, "text/plain", "File not found");

            return;
        }

        server.streamFile(file, "application/javascript");

        file.close();
    });

    server.on("/blocklist.js", HTTP_GET,[](){
        File file = LittleFS.open("/blocklist.js", "r");

        if(!file)
        {
            server.send(404, "text/plain", "File not found");

            return;
        }

        server.streamFile(file, "application/javascript");

        file.close();
    });

    server.on("/queries.js", HTTP_GET,[](){
        File file = LittleFS.open("/queries.js", "r");

        if(!file)
        {
            server.send(404, "text/plain", "File not found");

            return;
        }

        server.streamFile(file, "application/javascript");

        file.close();
    });

    server.on("/settings.js", HTTP_GET,[](){
        File file = LittleFS.open("/settings.js", "r");

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