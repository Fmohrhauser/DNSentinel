#include "time_manager.h"
#include <time.h>
#include "debug.h"

const char* ntpServer = "pool.ntp.org";

const long gmtOffset_sec = -21600;
const int daylightOffset_sec = 3600;

void initTime()
{
    configTime(
        gmtOffset_sec,
        daylightOffset_sec,
        ntpServer
    );

    DEBUG_PRINTLN("waiting for NTP time...");

    struct tm timeinfo;

    const unsigned long timeout = 10000;
    const unsigned long startTime = millis();

    while(!getLocalTime(&timeinfo))
    {
        if(millis() - startTime >= timeout)
        {
            DEBUG_PRINTLN("NTP syncronization timed out");
            return;
        }

        delay(500);
        DEBUG_PRINT(".");
    }

    DEBUG_PRINTLN();
    DEBUG_PRINTLN("Time synchronized");
}

String getCurrentTime()
{
    struct tm timeinfo;
    

    if(!getLocalTime(&timeinfo))
    {
        return "Unknown";
    }

    char buffer[30];

    strftime(
        buffer,
        sizeof(buffer),
        "%Y-%m-%d %H:%M:%S",
        &timeinfo
    );

    return String(buffer);
    
}