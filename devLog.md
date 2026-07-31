# DNSentinel Development Log

## Project Overview

DNSentinel is an ESP32-based DNS sinkhole designed to block unwanted domains at the network level.

The project combines embedded C++, networking, DNS protocol handling, caching, statistics, APIs, and a web dashboard hosted directly on the ESP32.

---

# Phase 1 — Project Foundation

## ESP32 Setup

- Configured ESP32-S3-WROOM-1 N16R8 development board
- Verified PSRAM functionality
- Migrated project to PlatformIO
- Improved project structure and organization

---

## Initial DNS Server

Implemented the core DNS sinkhole functionality:

- DNS request receiving
- DNS packet parsing
- Domain extraction
- Blocklist checking
- DNS blocking
- DNS forwarding
- DNS response generation

The first version successfully operated as a working DNS sinkhole.

---

# Phase 2 — Code Organization and Cleanup

## Modularization

Separated major firmware components into individual modules:

- DNS server
- Blocklist handling
- WiFi management
- Statistics tracking
- Query logging
- Settings management

This made the project easier to maintain and expand.

---

## Debug Improvements

Added:

- Compile-time debug logging
- Reduced unnecessary serial output
- Cleaner debugging workflow

---

# Phase 3 — DNS Features

## DNS Cache

Implemented a local DNS response cache.

Benefits:

- Faster repeat lookups
- Reduced upstream DNS traffic
- Improved response times

Added cache hit tracking separately from blocked and forwarded requests.

---

## Statistics System

Implemented DNS request tracking:

Tracked:

- Total requests
- Blocked requests
- Forwarded requests
- Cache hits

Created APIs to provide live statistics to the dashboard.

---

## Query Logging

Added recent DNS query logging.

Logs include:

- Requested domain
- Action taken

Actions:

- Blocked
- Forwarded
- Cache hit

---

# Phase 4 — Web Dashboard

## Dashboard Refactor

The first dashboard version was generated directly inside C++ strings.

This worked, but became difficult to manage as the dashboard grew.

Refactored the frontend:

- Moved HTML into LittleFS
- Separated CSS
- Separated JavaScript

This allows firmware and frontend development to happen independently.

---

## Dashboard Features

### Live Statistics API

Created API endpoints for:

- Total requests
- Blocked requests
- Forwarded requests
- Cache hits

The dashboard automatically updates statistics.

---

### Recent Query Display

Added a live table displaying:

- Domain
- Action

Added styling for:

- Blocked requests
- Forwarded requests
- Cache hits

---

### Request Breakdown Visualization

Added visual statistics bars:

- Forwarded percentage
- Blocked percentage
- Cache hit percentage

---

### System Information Footer

Added:

- ESP32 IP address
- Chip information
- Uptime
- Free memory

Added an option to hide the IP address when sharing screenshots publicly.

---

# Phase 5 — Settings System

## Settings Backend

Created a persistent configuration system.

Current settings:

- Blocking enabled
- Cache enabled
- Query logging enabled
- Upstream DNS server

Implemented:

- Settings struct
- Settings API
- ArduinoJson parsing
- LittleFS storage

---

## Settings API

Added:

### GET `/api/settings`

Returns current configuration.

### POST `/api/settings`

Updates settings from JSON input.

Settings are saved automatically after changes.

---

# Current Development

## Dashboard Settings Interface

Completed:

- Settings cog button
- Animated popup settings panel
- Save settings button
- Restore defaults button
- Settings status messages

Implemented:

- Persistent configuration storage
- Configuration API
- Browser-based settings control

Currently working on:

- Web-managed blocklist system
Goal:

Create a polished configuration interface similar to a commercial network appliance.

# Blocklist Management

Implemented a web-managed persistent blocklist system.

Features:

- View blocked domains from dashboard
- Add domains remotely
- Remove domains remotely
- Store blocklist in LittleFS
- Apply changes immediately without rebooting

The dashboard now allows users to manage DNS blocking rules directly from the device.

---

# Future Plans

## DNSentinel Software

Planned:

- Larger blocklists
- Better blocklist importing
- Wildcard blocklist support
- Local whitelist
- More DNS record types
- DNS-over-HTTPS support
- DNS-over-TLS support
- OTA updates
- Performance testing
- Memory profiling

---

## DNSentinel Hardware Addons

Future hardware projects:

- Custom PCB
- ESP32-P4 version
- ESP32-S31 version
- Ethernet support
- OLED status display
- Dedicated enclosure

The hardware version will build on top of the DNSentinel software platform.

---

# Skills Learned

DNSentinel has involved learning:

- Embedded C++
- ESP32 development
- DNS protocols
- UDP networking
- Network debugging
- REST APIs
- JSON parsing
- Web development
- Firmware architecture
- LittleFS storage
- Debugging hardware/software systems

