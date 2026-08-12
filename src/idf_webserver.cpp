#include "idf_webserver.h"
#include "debug.h"
#include "system.h"

httpd_handle_t idfServer = NULL;

esp_err_t testHandler(httpd_req_t *req)
{
    const char *response = "DNSentinel IDF server works";

    httpd_resp_send(
        req,
        response,
        HTTPD_RESP_USE_STRLEN
    );

    return ESP_OK;
}

esp_err_t helloHandler(httpd_req_t *req)
{
    const char *response ="{\"message\":\"Hello from IDF WebServer\"}";

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        response,
        HTTPD_RESP_USE_STRLEN
    );

    return ESP_OK;
}

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


httpd_uri_t testRoute = {
.uri = "/idf-test",
.method = HTTP_GET,
.handler = testHandler,
.user_ctx = NULL
};

httpd_uri_t helloRoute = {
    .uri = "/api/hello",
    .method = HTTP_GET,
    .handler = helloHandler,
    .user_ctx = NULL
};

httpd_uri_t systemAPIRoute = {
    .uri = "/api/system",
    .method = HTTP_GET,
    .handler = systemAPIHandler,
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
        idfServer, &testRoute 
    );
    httpd_register_uri_handler(
        idfServer, &helloRoute
    );
    httpd_register_uri_handler(
        idfServer, &systemAPIRoute
    );
}