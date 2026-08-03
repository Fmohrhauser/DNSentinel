#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>
#include <IPAddress.h>

//User Configuration
#define DEVICE_NAME "DNSentinel"
#define ENABLE_LOGGING true
#define ENABLE_CACHE true


constexpr size_t MAX_DNS_PACKET_SIZE = 512;


constexpr int CACHE_SIZE = 20;


extern const IPAddress UPSTREAM_DNS;


extern const uint16_t DNS_PORT;


extern const uint32_t UPSTREAM_TIMEOUT;

struct DeviceConfig
{
    String deviceName;

    IPAddress upstreamDNS;

    uint32_t UPSTREAM_TIMEOUT;

    bool cacheEnabled;

};

extern DeviceConfig deviceConfig;

void loadConfig();
void saveConfig();


#endif