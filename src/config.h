#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <IPAddress.h>

constexpr size_t MAX_DNS_PACKET_SIZE = 512;


constexpr int CACHE_SIZE = 20;


extern const IPAddress UPSTREAM_DNS;


extern const uint16_t DNS_PORT;


extern const uint32_t UPSTREAM_TIMEOUT;


#endif