# DNSentinel Architecture

This document describes the main DNSentinel modules, what each module is responsible for, and how they interact.

---

## Startup Flow

1. Initialize hardware and serial output
2. Start the file system `LittleFS`
3. Initialize the cache system
4. Initialize the query log system
5. Connect to WiFi
6. Start the DNS server system
7. Load the config properties
8. Initialize settings
9. Initialize the authentication system
10. Load saved settings
11. Start the dashboard/WebServer
12. Start API paths
13. Initialize current time check
14. Initialize the cache stats system
15. Load saved blocklist

---

## Modules

`include/api.h`

`src/api.cpp`

The api module stores API GET and POST functions that allow the dashboard to see and change information about the sinkhole.

Main Public Functions
- `startAPI()` - Starts the API paths for the Webserver

    **Depends On**
  - Dashboard
  - Auth
  - Modules that have trackable statistics: Query log, Dns Health, etc
  - Modules that have configurable settings: Blocklist, Settings, etc

    **Depended On By**
  - `Main.cpp` uses the startAPI command during startup
  
`auth.h`
`auth.cpp`

    The auth module stores functions responsible for checking if authentication is required for pages, aswell as checking if entered usernames and passwords are correct.

  `Main Public Functions`
  - `checkAuthentication()` - TODO
  - `hashPassword()` - TODO
 
    **Depends On**
  - Settings

    **Depended On By**
  - `api.cpp`

`blocked_stats.h`
`blocked_stats.cpp`

    The blocked stats module is responsible for collecting and storing blocked domain data.

  `Main Public Functions`
  - `incrementBlockedDomain()` - Adds a request to the count according to which domain was requested
  - `createTopBlockedJSON()` - Creates a JSON file containing the domains that are the most blocked

  **Depended On By**
  - `api.cpp`
  - `dashboard.cpp`
  - `dns_server.cpp`

`blocklist.h`
`blocklist.cpp`

    The blocklist module is responsible for checking if domains are blocked, aswell as updating(adding and removing) domains in the blocklist.

  `Main Public Functions`
  - `loadblocklist()` - TODO
  - `createBlocklistJSON()` - TODO
  - `importBlocklist()` - TODO
  - `getBlocklistSize()` - TODO
  - `clearBlocklist` - TODO

  **Depends on**
  - `hash table`

  **Depended On By**
  - `api.cpp`
  - `dns_server.cpp`
  - `main.cpp`

`cache_stats.h`
`cache_stats.cpp`

    The cache stats module is responsible for keeping track of cache related stats like cache hits for example.

  `Main Public Functions`
  - `initCacheStats()`
  - `recordCacheHit()`
  - `recordCacheMiss()`
  - `getCacheHitRate()`
  - `getAverageLookupTime()`
  - `createCacheStatsJSON()`

  **Depended On By**
  - `dns_cache.cpp`
  - `dns_server.cpp`
  - `main.cpp`

`config.h`
`config.cpp`

    The config module is responsible for controlling default/on startup system properties like upstream DNS DNS listening port and upstream request timeout.

  `Main Public Functions`
  - `loadConfig()` - TODO
  - `saveConfig()` - TODO

  **Depended On By**
  - `dns_server.cpp`

`dashboard.h`
`dashboard.cpp`
    The dashboard module is responsible for starting the main WebServer pages and their respective scripts and dependencies in the file system.

  `Main public functions`
  - `startDashboard()` - TODO
  - `handleDashboard` - TODO

  **Depends On**
  - stats
  - query log
  - system
  - blocked stats

  **Depended On By**
  - `main.cpp`

`debug.h`

    The debug module is responsible for the ability to toggle on/off debug prints in the serial monitor.

  `Main Public Functions`
  - `DEBUG_PRINT()` - TODO
  - `DEBUG_PRINTLN()` - TODO
  - `DEBUG_PRINTF()` - TODO

  **Depended On By**
  - Pretty much everything(especcially during the development phase)

`dns_cache.h`
`dns_cache.cpp`

    The dns cache module is responsible for managing(adding, removing, tombstone markers, etc) and reading/looking up the cached dns responses

  `Main Public Functions`
  - `cacheLookup()` - TODO
  - `cacheInsert()` - TODO
  - `cacheCleanup()` - TODO
  - `initCache()` - TODO
  
  **Depends On**
  - DNS parser
  - stats
  - cache_stats
  
  **Depended On By**
  - `dns_server.cpp`
  - `main.cpp`

`dns_health_stats.h`
`dns_health_stats.cpp`

    The dns health stats module is responsible for recording information about the health of the system like online/offline, latency, failures, requests, etc.

  `Main Public Functions`
  - `getUpstreamSuccessRate()` - TODO

  **Depended On By**
  - `api.cpp`
  - `dns_server.cpp`

`dns_health.h`
`dns_health.cpp`

    The dns health module is responsible for creating the JSON file responsible for storing all of the dns health stats data.

  `Main Public Functions`
  - `crateDNSHealthJSON()` - TODO

  **Depends on**
  - dns server
  - dns health stats

  **Depended On By**
  - `api.cpp`
