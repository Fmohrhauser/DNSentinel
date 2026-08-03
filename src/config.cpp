#include "config.h"

Preferences preferences;

DeviceConfig deviceConfig;


const IPAddress UPSTREAM_DNS(8,8,8,8);


const uint16_t DNS_PORT = 53;


const uint32_t UPSTREAM_TIMEOUT = 2000;


void loadConfig()
{
    preferences.begin("dnsentinel", false);

    if(preferences.isKey("name"))
    {
        deviceConfig.deviceName =
        preferences.getString(
                "name",
                "DNSentinel"
        );
    }
    else
    {
        deviceConfig.deviceName =
            "DNSentinel";
    }

    if(preferences.isKey("dns"))
    {
        String dns = 
            preferences.getString("dns");

        deviceConfig.upstreamDNS.fromString(dns);
    }
    else
    {
        deviceConfig.upstreamDNS.fromString("8.8.8.8");
    }



    deviceConfig.UPSTREAM_TIMEOUT =
        preferences.getUInt(
            "timeout",
            2000
        );

    deviceConfig.cacheEnabled =
        preferences.getBool(
            "cache",
            true
        );

    preferences.end();

    Serial.println("Config loaded");
}

void saveConfig()
{
    preferences.begin("dnsentinel", false);

    preferences.putString(
        "name",
        deviceConfig.deviceName
    );

    preferences.putString(
        "dns",
        deviceConfig.upstreamDNS.toString()
    );

    preferences.putUInt(
        "timeout",
        deviceConfig.UPSTREAM_TIMEOUT
    );

    preferences.putBool(
        "cache",
        deviceConfig.cacheEnabled
    );

    preferences.end();


    Serial.println("Config saved");
}
