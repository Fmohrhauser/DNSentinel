#include "auth.h"
#include "settings.h"
#include "mbedtls/sha256.h"


String hashPassword(String password)
{
    byte hash[32];

    mbedtls_sha256(
        (const unsigned char*)password.c_str(),
        password.length(),
        hash,
        0
    );

    String result="";

    for( int i=0;i<32;i++)
    {
        if(hash[i] < 16)
            result += "0";

        result += String(hash[i], HEX);
    }


    return result;
}

String decodeBase64(String input)
{
    const char* chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    String output = "";

    int val = 0;
    int valb = -8;

    for(char c : input)
    {
        if(c == '=')
            break;

        const char *found = strchr(chars, c);

        if(found == nullptr)
            continue;

        int index = found - chars;

        if(index < 0)
            continue;

        val = (val << 6) + index;

        valb += 6;

        if(valb >= 0)
        {
            output += char(
                (val >> valb) & 0xFF
            );

            valb -= 8;
        }
    }


    return output;
}

void sendAuthRequired(WebServer &server)
{
    server.sendHeader(
        "WWW-Authenticate",
        "Basic realm=\"DNSentinel\""
    );

    server.send(401, "application/json",
    "{\"error\":\"Authentication required\"}");
}


bool checkAuthentication(WebServer &server)
{
    Settings settings =
        getSettings();

    //Auth disabled
    if(!settings.authEnabled)
    {
        return true;
    }

    if(!server.hasHeader("Authorization"))
    {
        sendAuthRequired(server);
        return false;
    }

    String header = 
        server.header("Authorization");

    if(!header.startsWith("Basic "))
    {   
        sendAuthRequired(server);
        return false;
    }

    String encoded =
        header.substring(6);

    String decoded =
        decodeBase64(encoded);

    int separator =
        decoded.indexOf(':');

    if(separator < 0)
    {
        sendAuthRequired(server);
        return false;
    }

    String username =
        decoded.substring(
            0,
            separator
        );

    String password =
        decoded.substring(
            separator + 1
        );

    if(username != settings.username)
    {
        sendAuthRequired(server);
        return false;
    }

    String passwordHash =
        hashPassword(password);

    
    if(passwordHash != settings.passwordHash)
    {
        sendAuthRequired(server);
        return false;
    }

    return true;
}

void sendAuthRequiredIDF(httpd_req_t *req)
{
    httpd_resp_set_hdr(
        req,
        "WWW-Authenticate",
        "Basic realm= \"DNSentinel\""
    );

    httpd_resp_set_status(
        req,
        "401 Unauthorized"
    );

    httpd_resp_set_type(
        req,
        "application/json"
    );

    httpd_resp_send(
        req,
        "{\"error\":\"Authentication required\"}",
        HTTPD_RESP_USE_STRLEN
    );
}

bool checkAuthenticationIDF(httpd_req_t *req)
{
    Settings settings =
        getSettings();

    if(!settings.authEnabled)
    {
        return true;
    }

    size_t authLength = httpd_req_get_hdr_value_len(
        req,
        "Authorization"
    );

    if(authLength == 0)
    {
        sendAuthRequiredIDF(req);
        return false;
    }

    char authHeader[authLength + 1];

    esp_err_t headerResult =
        httpd_req_get_hdr_value_str(
            req,
            "Authorization",
            authHeader,
            authLength + 1
        );

    if(headerResult != ESP_OK)
    {
        sendAuthRequiredIDF(req);
        return false;
    }

    String header = authHeader;

    if(!header.startsWith("Basic "))
    {
        sendAuthRequiredIDF(req);
        return false;
    }

    String encoded =
        header.substring(6);

    String decoded = 
        decodeBase64(encoded);

    int separator =
        decoded.indexOf(':');

    if(separator < 0)
    {
        sendAuthRequiredIDF(req);
        return false;
    }

    String username =
        decoded.substring(
            0,
            separator
        );

    String password =
        decoded.substring(
            separator + 1
        );

    if(username != settings.username)
    {
        sendAuthRequiredIDF(req);
        return false;
    }

    String passwordHash =
        hashPassword(password);

    if(passwordHash != settings.passwordHash)
    {
        sendAuthRequiredIDF(req);
        return false;
    }

    return true;
}