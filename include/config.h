#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>
#include <IPAddress.h>

//User Configuration
#define DEVICE_NAME "DNSentinel"


constexpr size_t MAX_DNS_PACKET_SIZE = 512;
constexpr int CACHE_SIZE = 20;
const uint16_t DNS_PORT = 53;
const uint32_t UPSTREAM_TIMEOUT = 2000;


#endif