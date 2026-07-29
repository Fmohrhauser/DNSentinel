# DNSentinel

An ESP32-based DNS sinkhole designed to block unwanted domains at the network level.

## Current Features

- DNS packet parsing
- A and AAAA record support
- Domain blocking
- DNS response generation
- Upstream DNS forwarding
- DNS caching
- Query logging
- Request statistics
- Web dashboard hosted from LittleFS
- JSON API for live dashboard data

## Hardware

- ESP32-S3-WROOM-1 N16R8

## Dashboard

DNSentinel includes a web dashboard that provides:

- Live DNS statistics
- Recent DNS queries
- Blocked/forwarded status
- Browser-based monitoring

## Planned Features

- Larger blocklists
- Wildcard blocklist support
- Local whitelist
- Settings configuration through dashboard
- ESP32-P4 Ethernet version