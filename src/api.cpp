#include "dashboard.h"
#include "query_log.h"
#include "blocked_stats.h"
#include <WebServer.h>
#include "settings.h"
#include <ArduinoJson.h>
#include "blocklist.h"
#include "dns_health.h"
#include "system.h"
#include "dns_server.h"
#include "dns_health_stats.h"
#include "stats.h" 
#include "auth.h"


extern WebServer server;


void sendError(int code, const String &message)
{
    JsonDocument doc;
    doc["error"] = message;
    

    String output;
    serializeJson(doc, output);

    server.send(
        code,
        "application/json",
        output
    );
}

void sendStatus(int code, const String &status){
    JsonDocument doc;
    doc["status"] = status;

    String output;
    serializeJson(doc, output);

    server.send(
        code,
        "application/json",
        output
    );
}

void registerGetRoute(
    const char *path,
    String (*jsonFunction)(),
    bool requiresAuth
)
{
    server.on(path, HTTP_GET, [jsonFunction, requiresAuth](){

        if(requiresAuth && !checkAuthentication(server))
        {
            return;
        }

        server.send(
            200,
            "application/json",
            jsonFunction()
        );
    });
}




void startAPI()
{
    server.on("/api/auth/setup", HTTP_POST, [](){
        String body = server.arg("plain");

        JsonDocument doc;

        DeserializationError error =
            deserializeJson(doc, body.c_str());

        if (error)
        {
            sendError(
                400,
                "Invalid JSON"
            );

            return;
        }
        Settings settings = getSettings();
        if(settings.authEnabled){
            sendError(403, "Authentication already configured");
        }
        else if(!doc["username"].is<String>() ||
        !doc["password"].is<String>())
        {
            sendError(
                400,
                "Missing username or password"
            );

            return;
        }

        String username =
            doc["username"].as<String>();

        String password =
            doc["password"].as<String>();

        if(username.length() < 3 ||
            password.length() < 8)
        {
            sendError(
                400,
                "Username or password too short"
            );

            return;
        }

        settings.username =
            username;
        settings.passwordHash =
            hashPassword(password);

            settings.authEnabled =
                true;

            updateSettings(settings);

            sendStatus(
                200,
                "Authentication enabled"
            );
        
    });
    registerGetRoute(
        "/api/stats",
        createStatsJSON,
        false
    );

    server.on("/api/logs", HTTP_GET, []()
    {   
        if(!checkAuthentication(server))
            return;
        int limit = 25;

        if(server.hasArg("limit"))
        {
            limit = server.arg("limit").toInt();
        }

        if(limit < 1){
            limit = 1;
        }

        if(limit > MAX_QUERY_LOGS)
        {
            limit = MAX_QUERY_LOGS;
        }

        server.send(
            200,
            "application/json",
            createQueryLogJSON(limit)
        );
    });

    registerGetRoute(
        "/api/topblocked",
        createTopBlockedJSON,
        true
    );

    registerGetRoute(
        "/api/settings",
        createSettingsJSON,
        true
    );

    server.on("/api/settings", HTTP_POST, [](){

        if(!checkAuthentication(server)){
            return;
        }

        String body = server.arg("plain");


        JsonDocument doc;

        DeserializationError error = deserializeJson(doc, body.c_str());

        if(error)
        {
            sendError(
                400,
                "Invalid JSON"
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
            
            if(!validIP(dns))
            {
                sendError(
                    400,
                    "Invalid upstream DNS"
                );
            return;
            }
            newSettings.upstreamDNS = dns;
        }

        if(doc["blockingMode"].is<int>())
        {
            int mode = 
                doc["blockingMode"].as<int>();

            if(mode < 0 || mode > 2){
                sendError(
                    400,
                    "Invalid blocking mode"
                );
            return;
            }

            newSettings.blockingMode =
                static_cast<BlockingMode>(mode);
        }

        if(doc["redirectIP"].is<String>())
        {
            String ip =
                doc["redirectIP"].as<String>();
            if(validIP(ip))
            {
                newSettings.redirectIP = ip;
            }
            else
            {
               sendError(
                400,
                "Invalid redirect IP"
               );

                return;
            }
        }

        updateSettings(newSettings);


        sendStatus(
            200,
            "updated"
        );
    });

    server.on("/api/settings/reset", HTTP_POST,[](){
        if(!checkAuthentication(server))
            return;
        initializeSettings();

        saveSettings();

        sendStatus(
            200,
            "defaults restored"
        );
    });

    registerGetRoute(
        "/api/blocklist",
        createBlocklistJSON,
        false
    );

    server.on("/api/blocklist/add", HTTP_POST, [](){
        if(!checkAuthentication(server))
            return;
        String body = server.arg("plain");

        JsonDocument doc;

        DeserializationError error = 
            deserializeJson(doc, body.c_str());

            if(error)
            {
                sendError(
                    400,
                    "Invalid JSON"
                );

                return;
            }

            if(!doc["domain"].is<String>())
            {
                sendError(
                    400,
                    "Missing domains"
                );

                return;
            }

            String domain = 
                doc["domain"].as<String>();

                bool success = 
                    addBlockedDomain(domain);

                    if(success){
                        sendStatus(
                            201,
                            "added"
                        );
                    }
                    else
                    {
                        sendError(
                            409,
                            "Domain already exists"
                        );
                    }
    });

    server.on("/api/blocklist/remove", HTTP_POST, [](){
        if(!checkAuthentication(server))
            return;
        String body = server.arg("plain");

        JsonDocument doc;

        DeserializationError error =
            deserializeJson(doc, body.c_str());

        if(error){
            sendError(
                400,
                "Invalid JSON"
            );

            return;
        }

        if(!doc["domain"].is<String>())
        {
            sendError(
                400,
                "Missing domain"
            );

            return;
        }

        String domain=
            doc["domain"].as<String>();


        bool success =
            removeBlockedDomain(domain);
        if(success){
            sendStatus(
                200,
                "removed"
            );
        }
        else{
            sendError(
                404,
                "Domain not found"
            );
        }
    });


    registerGetRoute(
        "/api/dnshealth",
        createDNSHealthJSON,
        false
    );

    server.on("/api/blocklist/import", HTTP_POST,[](){
        if(!checkAuthentication(server))
            return;
        if(!server.hasArg("plain"))
        {
            sendError(
                400,
                "Missing data"
            );
            return;
        }

        JsonDocument doc;
        DeserializationError error =
            deserializeJson(
                doc,
                server.arg("plain")
            );

        if(error){
            sendError(
                400,
                "Invalid JSON"
            );
            return;
        }

        if(!doc["domains"].is<String>()){
            sendError(
                400,
                "Missing domains"
            );

            return;
        }

        String text = doc["domains"].as<String>();

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
        if(!checkAuthentication(server)){
            return;
        }
        clearBlocklist();

        server.send(
            200,
            "application/json",
            "{\"success\":true}"
        );
    });

    registerGetRoute(
        "/api/system",
        createSystemJSON,
        true
    );

    server.on("/api/logs/clear", HTTP_POST, [](){
        if(!checkAuthentication(server))
            return;
        clearLogs();

        sendStatus(
            200,
            "cleared"
        );
    });

}


