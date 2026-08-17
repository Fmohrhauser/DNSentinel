#include "auth.h"
#include "settings.h"
#include "mbedtls/sha256.h"
#include "esp_random.h"
#include <cstring>

const int MAX_AUTH_FAILURES = 5;
const unsigned long AUTH_LOCKOUT_MS = 30000;

int failedAuthAttempts = 0;
unsigned long authLockoutUntil = 0;
unsigned long authLockoutStarted = 0;
bool authLockoutActive = false;

bool authLockedOut()
{
    if(!authLockoutActive)
        return false;

    if(millis() - authLockoutStarted >= authLockoutUntil)
    {
        authLockoutActive = false;
        failedAuthAttempts = 0;
        return false;
    }

    return true;
}

void recordAuthFailure()
{
    failedAuthAttempts++;

    if(failedAuthAttempts >= MAX_AUTH_FAILURES)
    {
        authLockoutUntil =
            millis() + AUTH_LOCKOUT_MS;
        authLockoutActive = true;
    }
}

void resetAuthFailures()
{
    failedAuthAttempts = 0;
    authLockoutUntil = 0;
    authLockoutActive = false;
}


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

String generatePasswordSalt()
{
    const int saltBytes = 16;
    uint8_t salt[saltBytes];

    for(int i = 0; i < saltBytes; i +=4)
    {
        uint32_t randomValue = esp_random();

        int remaining = saltBytes - i;
        int bytesToCopy = remaining < 4 ? remaining : 4;
        
        memcpy(
            salt + i,
            &randomValue,
            bytesToCopy
        );
    }

    String saltString;

    for(int i = 0; i < saltBytes; i++)
    {
        if(salt[i] < 16)
            saltString += "0";

        saltString += String(
            salt[i],
            HEX
        );
    }

    return saltString;
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
    
    if(authLockedOut())
    {
        httpd_resp_set_status(
            req,
            "429 Too Many Requests"
        );

        httpd_resp_set_type(
            req,
            "application/json"
        );

        httpd_resp_send(
            req,
            "{\"error\":\"Too many failed authentication attempts\"}",
            HTTPD_RESP_USE_STRLEN
        );

        return false;
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
        recordAuthFailure();
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
        recordAuthFailure();
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
        recordAuthFailure();
        sendAuthRequiredIDF(req);
        return false;
    }

    String passwordHash =
        hashPassword(settings.passwordSalt + password);

    if(passwordHash != settings.passwordHash)
    {
        recordAuthFailure();
        sendAuthRequiredIDF(req);
        return false;
    }
    resetAuthFailures();
    return true;
}