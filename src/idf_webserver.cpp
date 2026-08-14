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

esp_err_t blocklistcountAPIHandler(httpd_req_t *req)
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

esp_err_t whitelistcountAPIHandler(httpd_req_t *req)
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

esp_err_t querylogsAPIHandler(httpd_req_t *req)
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

esp_err_t authsetupAPIHandler(httpd_req_t *req)
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
    settings.passwordHash =
        hashPassword(password);
    settings.authEnabled = true;

    updateSettings(settings);

    sendStatusIDF(
        req,
        "200 OK",
        "Authentication enabled"
    );
    return ESP_OK;
}

esp_err_t clearlogsAPIHandler(httpd_req_t *req)
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

esp_err_t settingsresetAPIHandler(httpd_req_t *req)
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

esp_err_t blocklistresetAPIHandler(httpd_req_t *req)
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
esp_err_t whitelistresetAPIHandler(httpd_req_t *req)
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

httpd_uri_t blocklistcountAPIRoute = {
    .uri ="/api/blocklist/count",
    .method = HTTP_GET,
    .handler = blocklistcountAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t whitelistcountAPIRoute = {
    .uri ="/api/whitelist/count",
    .method = HTTP_GET,
    .handler = whitelistcountAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t querylogsAPIRoute = {
    .uri = "/api/logs",
    .method = HTTP_GET,
    .handler = querylogsAPIHandler,
    .user_ctx = NULL
};
//POST ROUTES

httpd_uri_t authsetupAPIRoute = {
    .uri = "/api/auth/setup",
    .method = HTTP_POST,
    .handler = authsetupAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t clearlogsAPIRoute = {
    .uri = "/api/logs/clear",
    .method = HTTP_POST,
    .handler = clearlogsAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t settingsresetAPIRoute = {
    .uri = "/api/settings/reset",
    .method = HTTP_POST,
    .handler = settingsresetAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t blocklistresetAPIRoute = {
    .uri = "/api/blocklist/reset",
    .method = HTTP_POST,
    .handler = blocklistresetAPIHandler,
    .user_ctx = NULL
};

httpd_uri_t whitelistresetAPIRoute = {
    .uri = "/api/whitelist/reset",
    .method = HTTP_POST,
    .handler = whitelistresetAPIHandler,
    .user_ctx = NULL
};


void startIDFWebServer()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 8080;
    config.max_uri_handlers = 20;
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
        idfServer, &blocklistcountAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &whitelistcountAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &querylogsAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &authsetupAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &clearlogsAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &settingsresetAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &blocklistresetAPIRoute
    );
    httpd_register_uri_handler(
        idfServer, &whitelistAPIRoute
    );
}