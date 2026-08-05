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
- Multiple blocking modes
- Web-based configuration
- Persistent settings storage
- Restore default settings
- System monitoring
  - IP address
  - Uptime
  - Free memory
  - Chip information
- Web dashboard hosted from LittleFS 
- Json APIs for live dashboard data
- Responsive dashboard and Queries page
- Query searching, filtering, sorting, and display limits
- Query-log clearing
- Active-page sidebar highlighting


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
- Blocklist management
- Responsive layouts for desktop and mobile devices

## Architecture

DNSentinel separates the firmware and dashboard frontend:

- ESP32 firmware handles DNS processing, blocking, caching, and statistics
- LittleFS stores dashboard files (HTML/CSS/JavaScript)
- JSON APIs provide live data to the frontend
- Project modules seperate DNS handling, caching, blocklists, statistics, settings, system monitoring, and web API behavior

This allows the dashboard and firmware to be developed independently.

## Current Development

The current development focus is:

- Redesigning the Blocklist and Settings pages
- Improving frontend error handling
- Completing authentication
- Reviewing network-device security
- Improving input validation
- Testing memory usage and performance
- Preparing documentation and setup instructions


## Planned Features

- Wildcard blocklist support
- Local whitelist
- Complete authentication for management endpoints
- Export and import configuration
- Security threat model
- Request and upload size limits
- Protection against request flooding
- Performance and stress testing
- User testing
- Stable version 1.0 release
- ESP32-P4 Ethernet version
- ESP32-S31 version
- Custom PCB
- OLED status display