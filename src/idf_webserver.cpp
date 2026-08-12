#include "idf_webserver.h"
#include "debug.h"
#include "system.h"
#include "stats.h"
#include "blocked_stats.h"
#include "settings.h"
#include "blocklist.h"
#include "dns_health.h"
#include "whitelist.h"

httpd_handle_t idfServer = NULL;



esp_err_t systemAPIHandler(httpd_req_t *req)
{
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




void startIDFWebServer()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 8080;

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
}