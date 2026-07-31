#include "api.h"
#include "stats.h"
#include "dashboard.h"
#include "query_log.h"
#include "blocked_stats.h"
#include <WebServer.h>
#include "settings.h"
#include <ArduinoJson.h>
#include "blocklist.h"
#include "dns_health.h"
#include "system.h"

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

    server.on("/api/topblocked", HTTP_GET, [](){
        
        server.send(
            200,
            "application/json",
            createTopBlockedJSON()
        );
    });

    server.on("/api/settings", HTTP_GET, [](){

        server.send(
            200,
            "application/json",
            createSettingsJSON()
        );
    });

    server.on("/api/settings", HTTP_POST, [](){

        String body = server.arg("plain");

        Serial.println("Received settings:");
        Serial.println(body);

        JsonDocument doc;

        DeserializationError error = deserializeJson(doc, body);

        if(error)
        {
            server.send(
                400,
                "application/json",
                "{\"error\":\"Invalid JSON\"}"
            );

            return;
        }

        Settings newSettings = getSettings();

        if(doc["blockingEnabled"].is<bool>())
        {
            newSettings.blockingEnabled =
                doc["blockingEnabled"];
        }

        if(doc["cacheEnabled"].is<bool>())
        {
            newSettings.cacheEnabled =
                doc["cacheEnabled"];
        }

        if(doc["queryLoggingEnabled"].is<bool>())
        {
            newSettings.queryLoggingEnabled =
                doc["queryLoggingEnabled"];
        }

        if(doc["upstreamDNS"].is<String>())
        {
            String dns =
                doc["upstreamDNS"].as<String>();
            
            if(dns.length() > 0)
            {
                newSettings.upstreamDNS = dns;
            }
        }
            Serial.print("New upstream DNS: ");
            Serial.println(newSettings.upstreamDNS);
        updateSettings(newSettings);
        Serial.println(getSettings().blockingEnabled);


        server.send(
            200,
            "application/json",
            "{\"status\":\"updated\"}"
        );
    });

    server.on("/api/settings/reset", HTTP_POST,[](){

        initializeSettings();

        saveSettings();

        Serial.println("Serttings restored to defaults");

        server.send(
            200,
            "application/json",
            "{\"status\":\"defaults restored\"}"
        );
    });

    server.on("/api/blocklist", HTTP_GET, [](){

        server.send(
            200,
            "application/json",
            createBlocklistJSON()
        );
    });

    server.on("/api/blocklist/add", HTTP_POST, [](){

        String body = server.arg("plain");

        JsonDocument doc;

        DeserializationError error = 
            deserializeJson(doc, body);

            if(error)
            {
                server.send(
                    400,
                    "application/json",
                    "{\"error\":\"Invalid JSON\"}"
                );

                return;
            }

            if(!doc["domain"].is<String>())
            {
                server.send(
                    300,
                    "application/json",
                    "{\"error\":\"Missing domain\"}"
                );

                return;
            }

            String domain = 
                doc["domain"].as<String>();

                bool success = 
                    addBlockedDomain(domain);

                    server.send(
                        200,
                        "application/json",
                        success ?
                        "{\"status\":\"added\"}" :
                        "{\"status\":\"already exists\"}"
                    );
    });

    server.on("/api/blocklist/remove", HTTP_POST, [](){

        String body = server.arg("plain");

        JsonDocument doc;

        deserializeJson(doc, body);


        String domain=
            doc["domain"].as<String>();


        bool success =
            removeBlockedDomain(domain);

        server.send(
            200,"application/json",
            success ?
            "{\"status\":\"removed\"}" :
            "{\"status\":\"not found\"}"
        );
    });


    server.on("/api/dnshealth", HTTP_GET, [](){

        server.send(
            200,
            "application/json",
            createDNSHealthJSON()
        );
    });

    server.on("/api/blocklist/import", HTTP_POST,[](){

        if(!server.hasArg("plain"))
        {
            server.send(400, "text/plain", "Missing data");
            return;
        }

        JsonDocument doc;

        deserializeJson(
            doc,
            server.arg("plain")
        );

        String text = doc["domains"];

        ImportResult result = importBlocklist(text);

        JsonDocument response;

        response["added"] = result.added;
        response["duplicates"] = result.duplicates;
        response["ignored"] = result.ignored;

        String output;

        serializeJson(response, output);

        server.send(
            200,
            "application/json",
            output
        );
    });

    server.on("/api/blocklist/count", HTTP_GET, [](){

        JsonDocument doc;

        doc["count"] = getBlocklistSize();

        String json;

        serializeJson(doc, json);

        server.send(
            200,
            "application/json",
            json
        );
    });

    server.on("/api/blocklist/reset", HTTP_POST, [](){

        clearBlocklist();

        server.send(
            200,
            "application/json",
            "{\"success\":true}"
        );
    });

    server.on("/api/system", HTTP_GET, [](){
        
        server.send(
            200,
            "application/json",
            createSystemJSON()
        );
    });
}