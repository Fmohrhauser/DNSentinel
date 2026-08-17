#include "idf_webserver.h"
#include "debug.h"
#include "system.h"
#include "stats.h"
#include "blocked_stats.h"
#include "settings.h"
#include "blocklist.h"
#include "dns_health.h"
#include "whitelist.h"
#include "auth.h"
#include <ArduinoJson.h>
#include "query_log.h"
#include <LittleFS.h>


httpd_handle_t idfServer = NULL;

void sendErrorIDF(
    httpd_req_t *req,
    const char *status,
    const String &message
)
{
    JsonDocument doc;
    doc["error"] = message;

    String output;
    serializeJson(doc, output);
    httpd_resp_set_status(
        req,
        status
    );

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        output.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
}

void sendStatusIDF(
    httpd_req_t *req,
    const char *statusCode,
    const String &status
)
{
    JsonDocument doc;
    doc["status"] = status;

    String output;
    serializeJson(doc, output);
    httpd_resp_set_status(req, statusCode);

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        output.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
}
String readRequestBody(httpd_req_t *req){
    int bodyLength = req->content_len;

    if(bodyLength <= 0)
        return "";

    char body[bodyLength + 1];

    int totalReceived = 0;
    while(totalReceived < bodyLength)
    {
        int received = httpd_req_recv(
            req,
            body + totalReceived,
            bodyLength - totalReceived
        );

        if(received <=0)
            return "";

        totalReceived += received;
    }
    
    body[totalReceived] = '\0';
    return String(body);
}

esp_err_t sendFileIDF(
    httpd_req_t *req,
    const char *path,
    const char *contentType
)
{
        if(!checkAuthenticationIDF(req))
        return ESP_OK;
    File file = LittleFS.open(path , "r");

    if(!file)
    {
        httpd_resp_set_status(
            req,
            "404 Not Found"
        );

        httpd_resp_set_type(
            req,
            "text/plain"
        );
        httpd_resp_send(
            req,
            "File not found",
            HTTPD_RESP_USE_STRLEN
        );

        return ESP_OK;
    }

    httpd_resp_set_type(
        req,
        contentType
    );

    char buffer[1024];

    while(file.available())
    {
        size_t bytesRead =
            file.readBytes(
                buffer,
                sizeof(buffer)
            );
        if(bytesRead > 0)
        {
            esp_err_t result =
                httpd_resp_send_chunk(
                    req,
                    buffer,
                    bytesRead
                );

            if(result != ESP_OK)
            {
                file.close();
                return result;
            }
        }
    }

    file.close();

    httpd_resp_send_chunk(
        req,
        NULL,
        0
    );

    return ESP_OK;
}

//File hosting
esp_err_t dashboardFileHandler(httpd_req_t *req)
{
    return sendFileIDF(
        req,
        "/index.html",
        "text/html"
    );
}

esp_err_t dashboardJSHandler(httpd_req_t *req)
{
    return sendFileIDF(
        req,
        "/dashboard.js",
        "application/javascript"
    );
}

esp_err_t styleCSSHandler(httpd_req_t *req)
{
    return sendFileIDF(
        req,
        "/style.css",
        "text/css"
    );
}

esp_err_t queriesFileHandler(httpd_req_t *req)
{
    return sendFileIDF(
        req,
        "/queries.html",
        "text/html"
    );
}

esp_err_t queriesJSHandler(httpd_req_t *req)
{
    return sendFileIDF(
        req,
        "/queries.js",
        "application/js"
    );
}

esp_err_t blocklistFileHandler(httpd_req_t *req)
{
    return sendFileIDF(
        req,
        "/blocklist.html",
        "text/html"
    );
}

esp_err_t blocklistJSHandler(httpd_req_t *req)
{
    return sendFileIDF(
        req,
        "/blocklist.js",
        "application/js"
    );
}

esp_err_t whitelistFileHandler(httpd_req_t *req)
{
    return sendFileIDF(
        req,
        "/whitelist.html",
        "text/html"
    );
}

esp_err_t whitelistJSHandler(httpd_req_t *req)
{
    return sendFileIDF(
        req,
        "/whitelist.js",
        "application/javascript"
    );
}

esp_err_t settingsFileHandler(httpd_req_t *req)
{
    return sendFileIDF(
        req,
        "/settings.html",
        "text/html"
    );
}

esp_err_t settingsJSHandler(httpd_req_t *req)
{
    return sendFileIDF(
        req,
        "/settings.js",
        "application/javascript"
    );
}

//GET Handlers
esp_err_t systemAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;

    String response = createSystemJSON();

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        response.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;
}

esp_err_t statsAPIHandler(httpd_req_t *req)
{
    String response = createStatsJSON();

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        response.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;
}

esp_err_t topblockedAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;
    String response = createTopBlockedJSON();

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        response.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;
}

esp_err_t settingsAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;

    String response = createSettingsJSON();

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        response.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;
}

esp_err_t blocklistAPIHandler(httpd_req_t *req)
{
    String response = createBlocklistJSON();

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        response.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;
}

esp_err_t dnshealthAPIHandler(httpd_req_t *req)
{
    String response = createDNSHealthJSON();

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        response.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;
}

esp_err_t whitelistAPIHandler(httpd_req_t *req)
{
    String response = createWhitelistJSON();

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        response.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;
}

esp_err_t blocklistCountAPIHandler(httpd_req_t *req)
{
    JsonDocument doc;
    doc["count"] = getBlocklistSize();
    String response;
    serializeJson(doc, response);

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        response.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;

}

esp_err_t whitelistCountAPIHandler(httpd_req_t *req)
{
    JsonDocument doc;
    doc["count"] = getWhitelistSize();
    String response;
    serializeJson(doc, response);

    httpd_resp_set_type(
        req,"application/json"
    );

    httpd_resp_send(
        req,
        response.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;
}

esp_err_t queryLogsAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;
    int limit = 25;

    size_t queryLength = 
        httpd_req_get_url_query_len(req);

    if(queryLength > 0){
        char query[queryLength + 1];

        esp_err_t queryResult =
            httpd_req_get_url_query_str(
                req,
                query,
                queryLength + 1
            );

        if(queryResult == ESP_OK)
        {
            char limitValue[12];

            esp_err_t valueResult =
                httpd_query_key_value(
                    query,
                    "limit",
                    limitValue,
                    sizeof(limitValue)
                );

            if(valueResult == ESP_OK){
                limit = String(limitValue).toInt();
            }
        }
    }

    if(limit < 1)
        limit = 1;
    
    if(limit > MAX_QUERY_LOGS)
        limit = MAX_QUERY_LOGS;
    String response = createQueryLogJSON(limit);

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        response.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;
        
    
}
//POST HANDLERS

esp_err_t authSetupAPIHandler(httpd_req_t *req)
{
    int bodyLength = req->content_len;

    char body[bodyLength + 1];

    int recieved = httpd_req_recv(
        req,
        body,
        bodyLength
    );

    if(recieved <= 0){
        return ESP_FAIL;
    }

    body[recieved] = '\0';

    String bodyString = body;

    JsonDocument doc;
    DeserializationError error =
        deserializeJson(
            doc,
            bodyString.c_str()
        );

    if(error)
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );

        return ESP_OK;
    }

    Settings settings = getSettings();
    if(settings.authEnabled){
        sendErrorIDF(
            req,
            "403 Forbidden",
            "Authentication already configured"
        );

        return ESP_OK;
    }
    else if(!doc["username"].is<String>() ||
    !doc["password"].is<String>())
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Missing username or password"
        );

        return ESP_OK;
    }

    String username =
        doc["username"].as<String>();

    String password =
        doc["password"].as<String>();

    if(username.length()<3 ||
        password.length() < 8)
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Username or password too short"
        );

        return ESP_OK;
    }

    settings.username =
        username;
    settings.passwordSalt =
        generatePasswordSalt();
    settings.passwordHash =
        hashPassword(
            settings.passwordSalt + password
        );
        
    settings.authEnabled = true;

    updateSettings(settings);

    sendStatusIDF(
        req,
        "200 OK",
        "Authentication enabled"
    );
    return ESP_OK;
}

esp_err_t clearLogsAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;

    clearLogs();

    sendStatusIDF(
        req,
        "200 OK",
        "cleared"
    );

    return ESP_OK;
}

esp_err_t settingsPOSTAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;

    String body = readRequestBody(req);

    if(body.length() == 0)
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );

        return ESP_OK;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, body);
    if(error){
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );

        return ESP_OK;
    }

    Settings newSettings = getSettings();

    if(doc["blockingEnabled"].is<bool>())
    {
        newSettings.blockingEnabled = doc["blockingEnabled"];
    }
    if(doc["cacheEnabled"].is<bool>())
    {
        newSettings.cacheEnabled = doc["cacheEnabled"];
    }
    if(doc["queryLoggingEnabled"].is<bool>())
    {
        newSettings.queryLoggingEnabled = doc["queryLoggingEnabled"];
    }
    if(doc["upstreamDNS"].is<String>())
    {
        String dns = doc["upstreamDNS"].as<String>();

        if(!validIP(dns))
        {
            sendErrorIDF(
                req,
                "400 Bad Request",
                "Invalid upstream DNS"
            );

            return ESP_OK;
        }
        newSettings.upstreamDNS = dns;
    }

    if(doc["blockingMode"].is<int>())
    {
        int mode = doc["blockingMode"].as<int>();

        if(mode < 0 || mode > 2){
            sendErrorIDF(
                req,
                "400 Bad Request",
                "Invalid blocking mode"
            );
            return ESP_OK;
        }
        newSettings.blockingMode =
            static_cast<BlockingMode>(mode);
    }
    if(doc["redirectIP"].is<String>())
    {
        String ip = doc["redirectIP"].as<String>();
        if(validIP(ip))
        {
            newSettings.redirectIP = ip;
        }
        else
        {
            sendErrorIDF(
                req,
                "400 Bad Request",
                "Invalid redirect IP"
            );

            return ESP_OK;
        }
    }

    updateSettings(newSettings);

    sendStatusIDF(
        req,
        "200 OK",
        "updated"
    );
    return ESP_OK;
}

esp_err_t settingsResetAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;
    initializeSettings();
    saveSettings();

    sendStatusIDF(
        req,
        "200 OK",
        "defaults restored"
    );
    return ESP_OK;
}

esp_err_t blocklistResetAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;

    clearBlocklist();

    httpd_resp_set_status(
        req,
        "200 OK"
    );

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        "{\"success\":true}",
        HTTPD_RESP_USE_STRLEN
    );

    return ESP_OK;
};
esp_err_t whitelistResetAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;
    
    clearWhitelist();

    httpd_resp_set_status(
        req,
        "200 OK"
    );

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        "{\"success\":true}",
        HTTPD_RESP_USE_STRLEN
    );

    return ESP_OK;
}

esp_err_t blocklistAddAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;

    String body = readRequestBody(req);

    if(body.length() == 0)
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );
        return ESP_OK;
    }

    JsonDocument doc;
    DeserializationError error =
        deserializeJson(doc, body);
    if(error)
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );

        return ESP_OK;
    }
    if(!doc["domain"].is<String>())
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Missing domain"
        );
        return ESP_OK;
    }

    String domain = doc["domain"].as<String>();

    bool success = addBlockedDomain(domain);
    if(success){
        sendStatusIDF(
            req,
            "201 Created",
            "added"
        );
    }
    else{
        sendErrorIDF(
            req,
            "409 Conflict",
            "Domain already exists"
        );
    }

    return ESP_OK;
}

esp_err_t whitelistAddAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;

    String body = readRequestBody(req);

    if(body.length() == 0)
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );
        return ESP_OK;
    }

    JsonDocument doc;

    DeserializationError error =
        deserializeJson(doc, body);
    if(error){
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );

        return ESP_OK;
    }
    if(!doc["domain"].is<String>())
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Missing domain"
        );
        return ESP_OK;
    }

    String domain = doc["domain"].as<String>();

    bool success = addWhitelistedDomain(domain);
    if(success){
        sendStatusIDF(
            req,
            "201 Created",
            "added"
        );
    }
    else{
        sendErrorIDF(
            req,
            "409 Conflict",
            "Domain already exists"
        );
    }

    return ESP_OK;
}

esp_err_t blocklistRemoveAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;
    String body = readRequestBody(req);

    if(body.length() == 0)
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );
        return ESP_OK;
    }

    JsonDocument doc;
    DeserializationError error =
        deserializeJson(doc, body);
    if(error){
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );

        return ESP_OK;
    }

    if(!doc["domain"].is<String>())
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Missing domain"
        );
        return ESP_OK;
    }

    String domain = doc["domain"].as<String>();

    bool success = removeBlockedDomain(domain);
    if(success){
        sendStatusIDF(
            req,
            "200 OK",
            "removed"
        );
    }
    else{
        sendErrorIDF(
            req,
            "404 Not Found",
            "Domain not found"
        );
    }
    return ESP_OK;

}

esp_err_t whitelistRemoveAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;
    String body = readRequestBody(req);

    if(body.length() == 0)
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );
        return ESP_OK;
    }

    JsonDocument doc;
    DeserializationError error =
        deserializeJson(doc, body);
    if(error){
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );

        return ESP_OK;
    }

    if(!doc["domain"].is<String>())
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Missing domain"
        );
        return ESP_OK;
    }

    String domain = doc["domain"].as<String>();

    bool success = removeWhitelistedDomain(domain);
    if(success){
        sendStatusIDF(
            req,
            "200 OK",
            "removed"
        );
    }
    else{
        sendErrorIDF(
            req,
            "404 Not Found",
            "Domain not found"
        );
    }
    return ESP_OK;
}

esp_err_t blocklistImportAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;
    String body = readRequestBody(req);

    if(body.length() == 0)
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );
        return ESP_OK;
    }

    JsonDocument doc;
    DeserializationError error =
        deserializeJson(doc, body);
    if(error){
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );
        return ESP_OK;
    }

    if(!doc["domains"].is<String>()){
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Missing domains"
        );

        return ESP_OK;
    }

    String text = doc["domains"].as<String>();

    ImportResultBlocklist result = importBlocklist(text);

    JsonDocument response;

    response["added"] = result.added;
    response["duplicates"] = result.duplicates;
    response["ignored"] = result.ignored;

    String output;

    serializeJson(response, output);

    httpd_resp_set_status(req, "200 OK");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(
        req,
        output.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;
}

esp_err_t whitelistImportAPIHandler(httpd_req_t *req)
{
    if(!checkAuthenticationIDF(req))
        return ESP_OK;
    String body = readRequestBody(req);

    if(body.length() == 0)
    {
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );
        return ESP_OK;
    }

    JsonDocument doc;
    DeserializationError error =
        deserializeJson(doc, body);
    if(error){
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Invalid JSON"
        );
        return ESP_OK;
    }

    if(!doc["domains"].is<String>()){
        sendErrorIDF(
            req,
            "400 Bad Request",
            "Missing domains"
        );
        return ESP_OK;
    }
    String text = doc["domains"].as<String>();

    ImportResultWhitelist result = importWhitelist(text);

    JsonDocument response;

    response["added"] = result.added;
    response["duplicates"] = result.duplicates;
    response["ignored"] = result.ignored;

    String output;

    serializeJson(response, output);

    httpd_resp_set_status(req, "200 OK");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(
        req,
        output.c_str(),
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;
}

//Static file routes
httpd_uri_t dashboardFileRoute = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = dashboardFileHandler,
    .user_ctx = NULL
};

httpd_uri_t dashboardJSRoute = {
    .uri = "/dashboard.js",
    .method = HTTP_GET,
    .handler = dashboardJSHandler,
    .user_ctx = NULL
};

httpd_uri_t styleCSSRoute = {
    .uri = "/style.css",
    .method = HTTP_GET,
    .handler = styleCSSHandler,
    .user_ctx = NULL
};

httpd_uri_t queriesFileRoute = {
    .uri = "/queries.html",
    .method = HTTP_GET,
    .handler = queriesFileHandler,
    .user_ctx = NULL
};

httpd_uri_t queriesJSRoute = {
    .uri = "/queries.js",
    .method = HTTP_GET,
    .handler = queriesJSHandler,
    .user_ctx = NULL
};

httpd_uri_t blocklistFileRoute = {
    .uri = "/blocklist.html",
    .method = HTTP_GET,
    .handler = blocklistFileHandler,
    .user_ctx = NULL
};

httpd_uri_t blocklistJSRoute = {
    .uri = "/blocklist.js",
    .method = HTTP_GET,
    .handler = blocklistJSHandler,
    .user_ctx = NULL
};

httpd_uri_t whitelistFileRoute = {
    .uri = "/whitelist.html",
    .method = HTTP_GET,
    .handler = whitelistFileHandler,
    .user_ctx = NULL
};

httpd_uri_t whitelistJSRoute = {
    .uri = "/whitelist.js",
    .method = HTTP_GET,
    .handler = whitelistJSHandler,
    .user_ctx = NULL
};

httpd_uri_t settingsFileRoute = {
    .uri = "/settings.html",
    .method = HTTP_GET,
    .handler = settingsFileHandler,
    .user_ctx = NULL
};

httpd_uri_t settingsJSRoute = {
    .uri = "/settings.js",
    .method = HTTP_GET,
    .handler = settingsJSHandler,
    .user_ctx = NULL
};
//GET ROUTES
httpd_uri_t systemAPIRoute = {
    .uri = "/api/system",
    .method = HTTP_GET,
    .handler = systemAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t statsAPIRoute = {
    .uri = "/api/stats",
    .method = HTTP_GET,
    .handler = statsAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t topblockedAPIRoute = {
    .uri = "/api/topblocked",
    .method = HTTP_GET,
    .handler = topblockedAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t settingsAPIRoute = {
    .uri = "/api/settings",
    .method = HTTP_GET,
    .handler = settingsAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t blocklistAPIRoute = {
    .uri = "/api/blocklist",
    .method = HTTP_GET,
    .handler = blocklistAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t dnshealthAPIRoute = {
    .uri = "/api/dnshealth",
    .method = HTTP_GET,
    .handler = dnshealthAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t whitelistAPIRoute = {
    .uri = "/api/whitelist",
    .method = HTTP_GET,
    .handler = whitelistAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t blocklistCountAPIRoute = {
    .uri ="/api/blocklist/count",
    .method = HTTP_GET,
    .handler = blocklistCountAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t whitelistCountAPIRoute = {
    .uri ="/api/whitelist/count",
    .method = HTTP_GET,
    .handler = whitelistCountAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t queryLogsAPIRoute = {
    .uri = "/api/logs",
    .method = HTTP_GET,
    .handler = queryLogsAPIHandler,
    .user_ctx = NULL
};
//POST ROUTES

httpd_uri_t authSetupAPIRoute = {
    .uri = "/api/auth/setup",
    .method = HTTP_POST,
    .handler = authSetupAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t clearLogsAPIRoute = {
    .uri = "/api/logs/clear",
    .method = HTTP_POST,
    .handler = clearLogsAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t settingsPOSTAPIRoute = {
    .uri = "/api/settings",
    .method = HTTP_POST,
    .handler = settingsPOSTAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t settingsResetAPIRoute = {
    .uri = "/api/settings/reset",
    .method = HTTP_POST,
    .handler = settingsResetAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t blocklistResetAPIRoute = {
    .uri = "/api/blocklist/reset",
    .method = HTTP_POST,
    .handler = blocklistResetAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t whitelistResetAPIRoute = {
    .uri = "/api/whitelist/reset",
    .method = HTTP_POST,
    .handler = whitelistResetAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t blocklistAddAPIRoute = {
    .uri = "/api/blocklist/add",
    .method = HTTP_POST,
    .handler = blocklistAddAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t whitelistAddAPIRoute = {
    .uri = "/api/whitelist/add",
    .method = HTTP_POST,
    .handler = whitelistAddAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t blocklistRemoveAPIRoute = {
    .uri = "/api/blocklist/remove",
    .method = HTTP_POST,
    .handler = blocklistRemoveAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t whitelistRemoveAPIRoute = {
    .uri = "/api/whitelist/remove",
    .method = HTTP_POST,
    .handler = whitelistRemoveAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t blocklistImportAPIRoute = {
    .uri = "/api/blocklist/import",
    .method = HTTP_POST,
    .handler = blocklistImportAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t whitelistImportAPIRoute = {
    .uri = "/api/whitelist/import",
    .method = HTTP_POST,
    .handler = whitelistImportAPIHandler,
    .user_ctx = NULL
};


void startIDFWebServer()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.max_uri_handlers = 35;
    config.stack_size = 8192;

    esp_err_t result = httpd_start(
        &idfServer,
        &config
    );

    if(result != ESP_OK)
    {
        DEBUG_PRINTLN("Failed to start IDF web server");
        return;
    }

    DEBUG_PRINTLN("IDF web server started");

    httpd_register_uri_handler(
        idfServer, &systemAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &statsAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &topblockedAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &settingsAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &blocklistAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &dnshealthAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &whitelistAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &blocklistCountAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &whitelistCountAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &queryLogsAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &authSetupAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &clearLogsAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &settingsPOSTAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &settingsResetAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &blocklistResetAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &whitelistResetAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &blocklistAddAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &whitelistAddAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &blocklistRemoveAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &whitelistRemoveAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &blocklistImportAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &whitelistImportAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &dashboardFileRoute
    );
    httpd_register_uri_handler(
        idfServer, &dashboardJSRoute
    );
    httpd_register_uri_handler(
        idfServer, &styleCSSRoute
    );
    httpd_register_uri_handler(
        idfServer, &queriesFileRoute
    );
    httpd_register_uri_handler(
        idfServer, &queriesJSRoute
    );
    httpd_register_uri_handler(
        idfServer, &blocklistFileRoute
    );
    httpd_register_uri_handler(
        idfServer, &blocklistJSRoute
    );
    httpd_register_uri_handler(
        idfServer, &whitelistFileRoute
    );
    httpd_register_uri_handler(
        idfServer, &whitelistJSRoute
    );
    httpd_register_uri_handler(
        idfServer, &settingsFileRoute
    );
    httpd_register_uri_handler(
        idfServer, &settingsJSRoute
    );
}