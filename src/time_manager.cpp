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

    Serial.println("waiting for NTP time...");

    struct tm timeinfo;

    while(!getLocalTime(&timeinfo))
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Time synchronized");
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