# DNSentinel

DNSentinel is an embedded DNS sinkhole built on the ESP32-S3, specifically the N16R8 development board variant. It acts as a local DNS server that can block unwanted domains before requests leave the network while providing a browser-based interface, hosted locally on the ESP32, for monitoring, configuration, blocklist management, and system statistics.

The project is designed as a self-contained network appliance rather than a simple DNS forwarding script. DNSentinel implements DNS packet handling, domain filtering, persistent configuration, statistics, response caching, authentication, and a web management interface directly on the microcontroller.

## Features

### DNS Filtering

- Local DNS server on UDP port 53
- DNS question parsing
- A and AAAA query handling
- Upstream DNS forwarding
- Configurable upstream DNS server
- Domain blocklist
- Domain whitelist
- Parent-domain blocking
- Configurable blocking behavior:
  - Null IP
  - NXDOMAIN
  - Redirect IP
- DNS response caching based on response TTL
- Runtime cache enable/disable setting
- Runtime blocking enable/disable setting

### Blocklist and Whitelist Management

DNSentinel stores blocklist and whitelist data in PSRAM-backed hash tables for fast domain lookups.

The management interface supports:

- Adding individual domains
- Removing individual domains
- Searching stored domains
- Paginated list viewing
- Clearing lists
- Importing newline-separated domain lists
- Normalization of common blocklist formats
- Duplicate detection
- Domain validation

Imported entries can include common formats such as:

~~~text
example.com
0.0.0.0 example.com
127.0.0.1 example.com
||example.com^
~~~

Domains are normalized before being inserted into the hash table.

Whitelist matches are checked before blocklist matches, allowing blocked domains to be explicitly permitted when necessary.

### DNS Cache

Forwarded upstream responses can be cached locally to reduce repeated upstream DNS requests.

The cache:

- Stores DNS responses by domain and query type
- Reads TTL values from upstream DNS responses
- Expires entries based on their TTL
- Restores the requesting client's DNS transaction ID before returning a cached response
- Tracks cache hits and misses
- Can be enabled or disabled from the settings interface

### Query Logging and Statistics

DNSentinel records runtime information related to DNS traffic, including:

- Total DNS requests
- Blocked requests
- Forwarded requests
- Cache hits
- Queries per minute
- Recent queries
- Query actions:
  - Blocked
  - Forwarded
  - Cache hit
- Most frequently blocked domains
- Upstream DNS availability
- Upstream DNS request failures
- Upstream DNS success rate
- Average successful upstream response latency
- Last successful upstream response

Query logging can be disabled independently from the DNS statistics system.

### Web Dashboard

DNSentinel includes a responsive browser-based management interface stored in LittleFS directly on the ESP32.

The interface contains pages for:

- Dashboard
- Recent queries
- Blocklist
- Whitelist
- Settings

The dashboard displays live information from the DNS server and system through JSON API endpoints.

The frontend is built with:

- HTML
- CSS
- JavaScript

The HTTP backend uses the ESP-IDF HTTP server rather than the Arduino `WebServer` implementation used during earlier development.

### Configuration

Settings are stored persistently in LittleFS and restored when the device boots.

Configurable settings currently include:

- Upstream DNS server
- DNS blocking
- DNS caching
- Query logging
- Blocking mode
- Redirect IP
- Authentication state and account information

### Authentication

The management interface supports HTTP Basic Authentication.

Authentication includes:

- SHA-256 password hashing
- Password changes
- Protection of management API endpoints
- Failed-login tracking
- Temporary lockout after repeated failed authentication attempts
- First-time username and password setup
- Persistent authentication configuration
- Random password salts generated using ESP32 hardware random data

Passwords are not stored directly in plaintext in the settings file.

> **Security note:** DNSentinel is currently intended for trusted local networks. HTTP Basic Authentication protects access to management endpoints, but HTTP traffic is not currently encrypted with TLS. TLS is planned for future versions. DNSentinel should not be exposed directly to the public internet.

---

# Hardware

The current DNSentinel implementation is developed for:

- **ESP32-S3-WROOM-1 N16R8**
- 16 MB flash
- 8 MB octal PSRAM
- Wi-Fi networking

The additional PSRAM is used primarily for the blocklist and whitelist domain hash tables.

The project is structured so that future versions can move toward more powerful ESP chips with additional features, including Ethernet-capable hardware such as the ESP32-P4. The current submitted implementation targets the ESP32-S3.

---

# Software

DNSentinel is built using:

- PlatformIO
- Arduino framework for ESP32
- ESP-IDF HTTP server
- LittleFS
- ArduinoJson
- ESP32 Wi-Fi and UDP networking
- mbedTLS SHA-256
- ESP32 PSRAM

The PlatformIO environment currently uses:

~~~ini
platform = espressif32@6.10.0
framework = arduino
board = esp32-s3-n16r8
~~~

---

# How It Works

A DNS request passes through multiple stages before DNSentinel responds.

~~~text
Client DNS Request
        |
        v
Parse DNS Question
        |
        v
Normalize Domain
        |
        v
Check Whitelist
        |
        v
Check Blocklist
        |
        +-------------------- Blocked ---------------------+
        |                                                  |
        |                                                  v
        |                                      Generate Blocking Response
        |                                      NULL / NXDOMAIN / Redirect
        |                                                  |
        |                                                  v
        |                                            Return to Client
        |
        v
Check DNS Cache
        |
        +-------------- Cache Hit -------------------------+
        |                                                  |
        |                                                  v
        |                                      Restore Transaction ID
        |                                                  |
        |                                                  v
        |                                            Return to Client
        |
        v
Forward Request to Upstream DNS
        |
        v
Receive Upstream Response
        |
        +----> Record DNS Health / Statistics
        |
        +----> Store Response in Cache
        |
        v
Return Response to Client
~~~

Whitelist checks occur before blocklist checks so an explicitly allowed domain is not blocked.

---

# Architecture

DNSentinel is separated into firmware modules that each have specific purposes rather than keeping all behavior inside the main program file.

~~~text
src/
├── auth.cpp
├── blocked_stats.cpp
├── blocklist.cpp
├── cache_stats.cpp
├── dns_cache.cpp
├── dns_health.cpp
├── dns_health_stats.cpp
├── dns_parser.cpp
├── dns_server.cpp
├── domain_utils.cpp
├── hash_table.cpp
├── idf_webserver.cpp
├── main.cpp
├── query_log.cpp
├── settings.cpp
├── stats.cpp
├── system.cpp
├── time_manager.cpp
├── whitelist.cpp
└── wifi_manager.cpp
~~~

## DNS Server

`dns_server.cpp` controls the main DNS request flow.

It is responsible for:

- Receiving UDP DNS requests
- Parsing the requested domain and record type
- Checking whitelist and blocklist state
- Applying the selected blocking mode
- Looking for cached responses
- Forwarding allowed requests upstream
- Returning responses to clients
- Updating logs and statistics

## DNS Parser

`dns_parser.cpp` handles DNS-specific packet parsing.

Its responsibilities include:

- Decoding domain names from DNS questions
- Extracting TTL values from DNS responses for caching

## Blocklist and Whitelist

`blocklist.cpp` and `whitelist.cpp` manage persistent domain lists.

Both use the custom `DomainHashTable` implementation for quick domain lookup while storing the domain strings in PSRAM so they do not overwhelm the normal heap.

Blocklist matching also checks parent domains.

For example, blocking:

~~~text
example.com
~~~

also allows DNSentinel to identify:

~~~text
ads.example.com
tracking.ads.example.com
~~~

as blocked without requiring every subdomain to be stored separately.

## Domain Hash Table

`hash_table.cpp` manages domain storage using an open-addressed hash table.

Each table entry contains:

- A domain hash
- An offset into the PSRAM string pool
- An entry state

The actual domain strings are stored in a linear pool in PSRAM.

The current configuration allocates approximately:

~~~text
Blocklist
65,536 hash-table slots
3 MB domain pool

Whitelist
8,192 hash-table slots
256 KB domain pool
~~~

Entries that are removed leave their previous domain data in the linear pool until the table is cleared or rebuilt. This is an intentional current limitation that helps avoid expensive pool compaction during individual removals.

## DNS Cache

`dns_cache.cpp` stores upstream responses for repeated queries.

Cache entries contain the requested domain, query type, response data, response length, and expiration time.

Expired entries are ignored and released for reuse.

## Query Log

`query_log.cpp` stores recent DNS activity for the Queries page.

Logging is separate from other statistics, allowing users to disable individual query recording while continuing to collect request counts.

## Statistics

Statistics are separated by responsibility:

- General DNS request statistics
- Blocked-domain statistics
- Cache statistics
- Upstream DNS health statistics

This allows the DNS processing path to remain relatively simple while still allowing the dashboard to display detailed runtime information.

## Settings

`settings.cpp` stores persistent configuration in:

~~~text
/settings.json
~~~

using LittleFS and ArduinoJson.

Default settings are initialized first and are then overwritten by saved values when available.

## Authentication

`auth.cpp` handles management-interface authentication.

Passwords are combined with a randomly generated salt before being hashed with SHA-256.

Repeated failed login attempts result in a temporary lockout.

## Web Server

`idf_webserver.cpp` contains the HTTP server, API handlers, and static-file handlers.

The server uses ESP-IDF's HTTP server APIs while the rest of the project continues to use the Arduino ESP32 framework. This was done for performance and to make future security improvements easier.

The frontend communicates with firmware through endpoints such as:

~~~text
/api/system
/api/stats
/api/topblocked
/api/settings
/api/blocklist
/api/whitelist
/api/logs
/api/dnshealth
~~~

Management operations such as removing or adding domains, importing lists of domains, clearing logs, changing settings, and changing authentication information use separate protected endpoints.

---

# Project Structure

~~~text
DNSentinel/
├── data/
│   ├── index.html
│   ├── dashboard.js
│   ├── queries.html
│   ├── queries.js
│   ├── blocklist.html
│   ├── blocklist.js
│   ├── whitelist.html
│   ├── whitelist.js
│   ├── settings.html
│   ├── settings.js
│   ├── style.css
│   ├── blocklist.txt
│   └── whitelist.txt
│
├── include/
│   └── Firmware headers
│
├── src/
│   └── Firmware source files
│
├── boards/
│   └── Custom PlatformIO board definition
│
├── platformio.ini
├── README.md
├── ARCHITECTURE.md
└── ROADMAP.md
~~~

---

# Building DNSentinel

## Requirements

You will need:

- ESP32-S3 N16R8 development board
- USB cable
- PlatformIO
- VS Code or another PlatformIO-compatible environment

Clone the repository and open the project directory in PlatformIO.

## Wi-Fi Credentials

Real Wi-Fi credentials are intentionally excluded from Git using `.gitignore` to prevent exposing them.

Copy:

~~~text
include/secrets.example.h
~~~

to:

~~~text
include/secrets.h
~~~

Then edit the new file:

~~~cpp
#pragma once

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
~~~

Do not commit your real `secrets.h`.

## Build Firmware

From the PlatformIO terminal:

~~~bash
pio run
~~~

## Upload LittleFS Data

The web interface and default domain-list files are stored in the `data/` directory.

Upload the filesystem image with:

~~~bash
pio run --target uploadfs
~~~

## Upload Firmware

~~~bash
pio run --target upload
~~~

## Serial Monitor

DNSentinel uses a baud rate of:

~~~text
115200
~~~

Open the serial monitor with:

~~~bash
pio device monitor
~~~

During startup, the device prints its Wi-Fi connection status and local IP address. This address will be used later when configuring DNS.

---

# Network Setup

After DNSentinel boots, note the ESP32's IP address from the Serial Monitor.

To use DNSentinel, configure a device or router to use that IP address as its primary DNS server.

Before changing the DNS server for an entire network, it is recommended to test DNSentinel on a single device or computer.

Once DNS traffic is pointed at DNSentinel:

1. The client sends DNS requests to the ESP32.
2. DNSentinel applies whitelist/blocklist rules.
3. Allowed queries are served from cache when possible.
4. Remaining requests are forwarded to the configured upstream DNS server.
5. DNSentinel returns the resulting DNS response to the client.

Open the ESP32's IP address in a browser to access the management interface.

---

# Blocking Modes

DNSentinel currently supports three blocking modes.

## Null IP

Blocked IPv4 requests receive:

~~~text
0.0.0.0
~~~

Blocked IPv6 requests receive an all-zero IPv6 address.

## NXDOMAIN

DNSentinel returns a response indicating that the requested domain does not exist.

## Redirect

Blocked IPv4 requests are redirected to a user-configurable IPv4 address.

For record types where DNSentinel does not construct a dedicated blocking response, it returns NXDOMAIN instead.

---

# Persistent Storage

DNSentinel uses the LittleFS filesystem for persistent data.

Persistent files include:

~~~text
/settings.json
/blocklist.txt
/whitelist.txt
~~~

This allows configuration and custom domain lists to survive device restarts.

Authentication information is stored as part of the persistent settings, while plaintext passwords are not stored.

The filesystem can be erased using PlatformIO's **Erase Flash** function. This resets the blocklist, whitelist, settings, and other persistent configuration to the defaults defined by the project. Authentication configuration is also cleared.

---

# Design Decisions

## ESP32 Instead of a Linux SBC

DNS filtering software is commonly run on Linux computers such as Raspberry Pis.

DNSentinel intentionally explores a different approach by implementing the appliance directly on a microcontroller.

My main motivations for doing this were cost and the challenge. ESP32 boards are significantly cheaper than Raspberry Pis, and I also wanted the project to push me to learn more about networking and embedded systems.

This required building or integrating systems that would normally be provided by a full operating system, including:

- DNS request handling
- Response forwarding
- DNS caching
- Persistent configuration
- Domain lookup structures
- HTTP APIs
- Authentication
- Runtime statistics

The result uses significantly smaller and cheaper hardware while still providing a complete local management interface.

## PSRAM-Backed Domain Storage

A large blocklist cannot reasonably be represented as thousands of independent heap-allocated `String` objects on an ESP32 because there is not enough internal memory for that approach.

DNSentinel therefore uses a custom hash table and a linear PSRAM domain pool.

This helps keep domain lookup fast while simultaneously moving the largest data structure away from internal RAM.

## Hybrid Arduino / ESP-IDF Design

DNSentinel uses the Arduino framework for much of the firmware because it provides convenient ESP32 networking and filesystem abstractions.

The HTTP layer uses ESP-IDF directly.

This provides access to ESP-IDF's lightweight HTTP server while avoiding the need to rewrite the entire project around native ESP-IDF.

## Separate Frontend and Firmware

The interface is stored as normal HTML, CSS, and JavaScript files instead of embedding large HTML strings into C++.

This keeps the code cleaner and easier to read while separating UI development from firmware logic and making the frontend easier to maintain.

---

# Current Limitations

DNSentinel is a working project, but it is not intended to replace a hardened production DNS resolver.

Current limitations include:

- Wi-Fi only on the current ESP32-S3 hardware
- No HTTPS/TLS for the management interface
- HTTP Basic Authentication should therefore only be used on trusted local networks
- DNS response handling is intentionally focused on the record types needed by the current sinkhole
- DNS cache size is currently fixed at compile time
- The hash-table domain pool is reclaimed when the table is cleared/rebuilt rather than after every individual deletion
- Network configuration currently requires manually setting the client/router DNS server to the ESP32 address
- Additional long-duration stress and malformed-packet testing would be valuable before production use

These limitations are kept explicit rather than hiding unfinished or unsupported behavior.

---

# Future Work

The next major directions for DNSentinel are:

- ESP32-P4 / Ethernet hardware version
- More extensive DNS protocol support
- HTTPS/TLS investigation
- Larger-scale stress and reliability testing
- Improved memory profiling and optimization
- Configuration import/export
- OTA firmware updates
- Additional network-security hardening
- Custom PCB and dedicated appliance hardware

---

# What I Learned

DNSentinel began as a DNS blocking project that I was inspired to make after seeing a video about Pi-hole on YouTube, but it grew into a much broader embedded-systems challenge.

Building it required working with:

- Binary network protocols
- UDP networking
- DNS packet structure
- Hash tables
- PSRAM memory management
- Filesystems
- Persistent configuration
- JSON APIs
- Frontend JavaScript
- Authentication
- Password hashing
- ESP-IDF APIs
- Runtime statistics
- Defensive input validation
- Debugging interactions between multiple subsystems

One of the biggest engineering challenges for me was balancing features with the extreme memory and processing constraints of a microcontroller, especially compared with a Raspberry Pi. Moving large domain lists into a custom PSRAM-backed structure, separating modules so that each had its own responsibility, and migrating the HTTP server to ESP-IDF were all changes made as the project grew.

The current version represents a functional embedded DNS appliance rather than only a proof of concept.

---

# Status

**Current target:** ESP32-S3 N16R8  
**Current stage:** Functional submission build

Core DNS filtering, caching, blocklist/whitelist management, persistent settings, authentication, statistics, and the web management interface are implemented and integrated.