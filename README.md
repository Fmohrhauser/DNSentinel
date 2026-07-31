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
- Cache hit tracking
- Top blocked domain analytics
- Web dashboard hosted from LittleFS
- JSON API for live dashboard data
- System monitoring (IP address, uptime, memory usage)
- Web-based configuration
- Persistent settings storage
- Restore default settings
- Configurable upstream DNS server

## Hardware

- ESP32-S3-WROOM-1 N16R8

## Dashboard

DNSentinel includes a web dashboard hosted directly from the ESP32 using LittleFS.

The dashboard provides:

- Live DNS statistics
- Request breakdown visualization
- Recent DNS queries
- Top blocked domains
- Cache hit statistics
- System information
- Browser-based monitoring
- Settings management
- Browser based configuration

## Architecture

DNSentinel separates the firmware and dashboard frontend:

- ESP32 firmware handles DNS processing, blocking, caching, and statistics
- LittleFS stores dashboard files (HTML/CSS/JavaScript)
- JSON APIs provide live data to the frontend

This allows the dashboard and firmware to be developed independently.

## Planned Features

- Larger blocklists
- Local whitelist
- Dashboard settings configuration
- Persistent configuration storage
- ESP32-P4 Ethernet version
- Custom PCB