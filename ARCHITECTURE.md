# DNSentinel Architecture

This document describes the main DNSentinel modules, what each module is responsible for, and how they interact.

---

## Startup Flow

1. Initialize hardware and serial output
2. Start the file system `LittleFS`
3. Initialize the cache system
4. Initialize the query log system
5. Connect to WiFi
6. Start the DNS UDP server
7. Load the saved device config
8. Initialize default settings
9. Initialize the default authentication settings
10. Load saved settings from LittleFS
11. Configure the WebServer to collect the `Authorization` header
12. Start the dashboard/WebServer routes
13. Start API routes
14. Synchronize system time using NTP
15. Initialize cache statistics
16. Initialize the blocklist hash table
17. Initialize the whitelist hash table
18. Load the saved blocklist
19. Load the saved whitelist

During the main loop:

1. Handle incoming dashboard/WebServer requests
2. Handle incoming DNS requests
3. Check for development/debug serial commands

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
  - `checkAuthentication()` - Checks whether authentication is enabled and validates the HTTP Basic Authentication username and password when required
  - `hashPassword()` - Creates a SHA-256 hash of a password for comparison/storage
 
    **Depends On**
  - Settings
  - WebServer

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
  - `loadBlocklist()` - Loads saved domains from `/blocklist.txt` into the blocklist hash table
  - `saveBlocklist()` - Saves the current blocklist hash table to LittleFS
  - `addBlockedDomain()` - Normalizes and adds a domain to the blocklist, then saves it
  - `removeBlockedDomain()` - Removes a domain from the blocklist and saves the updated list
  - `createBlocklistJSON()` - Creates a JSON array containing all currently blocked domains
  - `isBlocked` - Checks a domain and its parent domains against the blocklist
  - `importBlocklist()` - Imports and normalizes multiple new line seperated domains
  - `getBlocklistSize()` - Returns the number of domains currently in the blocklist
  - `clearBlocklist` - Clears the blocklist and saves the empty state

  **Depends on**
  - Hash table
  - Domain utils
  - LittleFS

  **Depended On By**
  - `api.cpp`
  - `dns_server.cpp`
  - `main.cpp`

`cache_stats.h`
`cache_stats.cpp`

    The cache stats module is responsible for keeping track of cache related stats like cache hits for example.

  `Main Public Functions`
  - `initCacheStats()` - Resets all cache lookup statistics
  - `recordCacheHit()` - Records a successful cache lookup and its lookup time
  - `recordCacheMiss()` - Records a failed cache lookup and its lookup time
  - `getCacheHitRate()` - calculates the percentage of cache lookups that resulted in hits
  - `getAverageLookupTime()` - Calculates the average cache lookup time
  - `createCacheStatsJSON()` - Creates a JSON object containing cache statistics

  **Depended On By**
  - `dns_cache.cpp`
  - `main.cpp`

`config.h`

    The config module stores  device configuration using ESP32 Preferences/NVS. it currently stores upstream DNS, timeout, and cache setting.


  **Depends On**
  - ESP32 Preferences

  **Depended On By**
  - `dns_server.cpp`

  **Current Code Note**

  Some configuration overlaps with the newer `settings` module. For example, the DNS server currently gets its upstream DNS from `Settings` rather than `DeviceConfig`

`dashboard.h`
`dashboard.cpp`
    The dashboard module owns the HTTP `WebServer` and serves the frontend HTML, JavaScript, CSS, and other static files stored in LittleFS

  `Main public functions`
  - `startDashboard()` - Registers frontend routes, streams files from LittleFS, and starts the HTTP server
  - `handleDashboard` - Processes incoming HTP client requests

  **Depends On**
  - WebServer
  - LittleFS


  **Depended On By**
  - `main.cpp`
  - `api.cpp` uses the WebServer instance created by this module

`debug.h`

    The debug module is responsible for the ability to toggle on/off debug prints in the serial monitor.

  `Main Public Functions`
  - `DEBUG_PRINT()` - Prints debug output when the `DEBUG` build flag is enabled
  - `DEBUG_PRINTLN()` - Prints debug output with a newline when debigguing is enabled
  - `DEBUG_PRINTF()` - Provides formatted debug printing when debugging is enabled

  **Depended On By**
  - Multiple modules throughout DNSentinel, especially DNS, cache, logging, and development/debug systems

`dns_cache.h`
`dns_cache.cpp`

    The dns cache module is responsible for managing(adding, removing, tombstone markers, etc) and reading/looking up the cached dns responses

  `Main Public Functions`
  - `cacheLookup()` - Marks all DNS cache entries as unused during startup
  - `cacheInsert()` - Searches the cache for a non-expired response matching a domain and query type
  - `cacheCleanup()` - Stores an upstream DNS response and its TTL in the cache
  - `initCache()` - Removes expired entries from the cache
  
  **Depends On**
  - DNS parser for response TTLs
  - stats
  - cache_stats
  - Config for cache size and maximum DNS packet size
  
  **Depended On By**
  - `dns_server.cpp`
  - `main.cpp`

`dns_health_stats.h`
`dns_health_stats.cpp`

    The dns health stats module stores runtime information about communication with the upstream DNS server, including requests, failures, latentcy, online status, and last successful response.

  `Main Public Functions`
  - `getUpstreamSuccessRate()` - Calculates the percentage of upstream DNS requests that succeeded

  **Depended On By**
  - `dns_cache.cpp`
  - `dns_server.cpp`

`dns_health.h`
`dns_health.cpp`

    The dns health module is responsible for converting upstream DNS health information into JSON for use by the API/dashboard.

  `Main Public Functions`
  - `crateDNSHealthJSON()` - Creates a JSON object containing upstream DNS status, requests, failures, success rate, latency, and last successful request

  **Depends on**
  - dns health stats

  **Depended On By**
  - `api.cpp`

`dns_parser.h`
`dns_parser.cpp`

The dns parser module is responsible for reading important information out of raw DNS packets, mainly requested domain names and TTL values from DNS responses

  `Main Public Functions`
  - `readDomain()` - Reads the encoded domain name from a DNS question and turns it into a nomral domain string
  - `getDNSResponseTTL()` - Reads the TTL from a DNS response so the cache knows how long the response can be stored

  **Depended On By**
  - `dns_server.cpp`
  - `dns_cache.cpp`

`dns_server.h`
`dns_server.cpp`

  The dns server module is the main DNS request handling system. it recieves DNS requests, parses them, checks the whitelist and blocklist, checks the cache, forwards allowed requests upstream, creates blocked responses, and records statistics/logs.

  `Main Public Functions`
  - `startDNSServer()` - Starts the local DNS UDP server on port 53 and initializes the UDP connection used for upstream DNS requests
  - `handleDNS` - Handles incoming DNS requests and controls the main DNS processing flow
  - `sendNXDOMAIN()` - Sends an NXDOMAIN response when NXDOMAIN blocking mode is selected

  **Depends On**
  - DNS parser
  - Blocklist
  - Whitelist
  - DNS cache
  - Query log
  - Stats
  - Blocked stats
  - Settings
  - DNS health stats
  - Config
  - WiFi UDP
  - Debug
  
  **Depended On By**

- `main.cpp`
- DNS health information is based on upstream state updated by this module

The main DNS request flow is roughly:

    Receive DNS packet
    ↓
    Parse requested domain and query type
    ↓
    Check exact whitelist
    ↓
    Check blocklist and parent domains
    ↓
    If blocked -> create response based on blocking mode
    ↓
    If allowed -> check DNS cache
    ↓
    Cache hit -> return cached response
    ↓
    Cache miss -> forward request to upstream DNS
    ↓
    Cache successful upstream response
    ↓
    Return response to client

---

`domain_utils.h`  
`domain_utils.cpp`

The domain utils module stores shared functions for cleaning and validating domains before they are used by filtering modules.

`Main Public Functions`

- `normalizeDomain()` - Cleans a domain, converts it to lowercase, removes supported blocklist/hosts-file formatting, ignores comments, and rejects invalid characters

**Depended On By**

- `blocklist.cpp`
- `whitelist.cpp`

---

`hash_table.h`  
`hash_table.cpp`

The hash table module provides the reusable `DomainHashTable` class used to quickly store, find, and remove domains.

Each hash table contains up to 4096 table entries and allocates its table in PSRAM when `begin()` is called.

The table uses three entry states:

- `EMPTY`
- `OCCUPIED`
- `DELETED`

The deleted state acts as a tombstone so removing a domain does not break later hash table lookups.

`Main Public Functions`

- `DomainHashTable()` - Creates a new hash table object with no allocated table yet
- `begin()` - Allocates and initializes the hash table in PSRAM
- `add()` - Adds a domain to the table if it does not already exist
- `contains()` - Checks whether a domain exists in the table
- `remove()` - Removes a domain and marks its entry as deleted
- `clear()` - Clears all entries from the table
- `size()` - Returns the number of domains currently stored
- `get()` - Returns a stored domain based on its position in the list of occupied entries

**Depended On By**

- Blocklist
- Whitelist

---

`query_log.h`  
`query_log.cpp`

The query log module stores recent DNS requests and information about what DNSentinel did with them.

Each query log entry contains:

- Domain
- Action
- Timestamp

Possible query actions are:

- `BLOCKED`
- `CACHE_HIT`
- `FORWARDED`

The query log currently stores up to 100 entries and wraps around when the log becomes full.

`Main Public Functions`

- `initQueryLog()` - Resets the query log during startup
- `logQuery()` - Adds a DNS request, its action, and the current time to the log
- `getQueryCount()` - Returns the current number of stored query logs
- `getLatestQueries()` - Gets the newest query log entries
- `actionToString()` - Converts a query action enum into readable text
- `createQueryLogJSON()` - Creates JSON containing recent query logs for the API
- `clearLogs()` - Clears all currently stored query logs

`Debug Functions`

- `printLogs()` - Prints stored query logs through the debug system
- `testLatest()` - Prints the latest queries for development/testing

**Depends On**

- Time manager
- Debug

**Depended On By**

- `dns_server.cpp`
- `api.cpp`
- `main.cpp`

---

`settings.h`  
`settings.cpp`

The settings module manages DNSentinel's main runtime settings and authentication settings. It stores these settings in `/settings.json` using LittleFS so they persist after restarting the ESP32.

Settings currently include:

- Upstream DNS
- Blocking enabled
- Cache enabled
- Query logging enabled
- Blocking mode
- Redirect IP
- Authentication enabled
- Username
- Password hash

`Main Public Functions`

- `initializeSettings()` - Loads the default DNS/filtering settings into memory
- `initializeAuthentication()` - Loads the default authentication state into memory
- `getSettings()` - Returns the current settings
- `updateSettings()` - Replaces the current settings and saves them
- `createSettingsJSON()` - Creates JSON containing settings that can be displayed by the API/dashboard
- `saveSettings()` - Saves the current settings to `/settings.json`
- `loadSettings()` - Loads saved settings from `/settings.json`
- `validIP()` - Checks whether a string is a valid IPv4 address

**Depends On**

- LittleFS
- ArduinoJson

**Depended On By**

- `main.cpp`
- `api.cpp`
- `auth.cpp`
- `dns_server.cpp`

---

`stats.h`  
`stats.cpp`

The stats module keeps track of general DNS request statistics such as total, blocked, forwarded, and cached requests.

It also calculates values used by the dashboard such as percentages and queries per minute.

`Main Public Functions`

- `initStats()` - Resets and initializes DNS statistics
- `resetStats()` - Resets the current statistics
- `incrementTotalRequests()` - Records a received DNS request
- `incrementBlockedRequests()` - Records a blocked DNS request
- `incrementForwardedRequests()` - Records a forwarded DNS request
- `incrementCacheHits()` - Records a DNS cache hit
- `getCacheHits()` - Returns the current number of cache hits
- `getBlockedPercent()` - Calculates the percentage of requests that were blocked
- `getForwardedPercent()` - Calculates the percentage of requests that were forwarded
- `getCachePercent()` - Calculates the percentage of requests served from cache
- `getQueriesPerMinute()` - Returns the number of queries recorded during the previous minute
- `getAverageQueriesPerSecond()` - Calculates the average DNS request rate since startup
- `getSuccessRate()` - Calculates the current forwarded request success percentage
- `createStatsJSON()` - Creates JSON containing the main DNS statistics for the API/dashboard

**Depended On By**

- `dns_server.cpp`
- `dns_cache.cpp`
- `api.cpp`

---

`system.h`  
`system.cpp`

The system module creates information about the ESP32 and DNSentinel's current runtime state for use by the API/dashboard.

`Main Public Functions`

- `createSystemJSON()` - Creates JSON containing the ESP32 IP address, chip model, uptime, and available heap memory

**Depends On**

- WiFi
- ArduinoJson
- ESP32 system information

**Depended On By**

- `api.cpp`

---

`time_manager.h`  
`time_manager.cpp`

The time manager module synchronizes the ESP32 clock using an NTP server and provides formatted timestamps for other modules.

`Main Public Functions`

- `initTime()` - Synchronizes the ESP32 system time with `pool.ntp.org`
- `getCurrentTime()` - Returns the current time as a formatted date/time string or `Unknown` if the time cannot be read

**Depends On**

- NTP/system time
- Debug
- An active network connection

**Depended On By**

- `main.cpp`
- `query_log.cpp`

---

`whitelist.h`  
`whitelist.cpp`

The whitelist module manages domains that should bypass DNSentinel's blocklist.

Whitelist matching is currently exact, meaning whitelisting a domain does not automatically whitelist its subdomains.

`Main Public Functions`

- `loadWhitelist()` - Loads saved whitelist domains from `/whitelist.txt` into the whitelist hash table
- `saveWhitelist()` - Saves the current whitelist to `/whitelist.txt`
- `addWhitelistedDomain()` - Normalizes and adds a domain to the whitelist, then saves it
- `removeWhitelistedDomain()` - Removes a domain from the whitelist and saves the updated list
- `createWhitelistJSON()` - Creates a JSON array containing all whitelisted domains
- `isWhitelisted()` - Checks whether an exact domain exists in the whitelist
- `importWhitelist()` - Imports and normalizes multiple newline-separated whitelist domains
- `getWhitelistSize()` - Returns the number of domains currently whitelisted
- `clearWhitelist()` - Clears all whitelist domains and saves the empty state

**Depends On**

- Hash table
- Domain utils
- LittleFS
- ArduinoJson

**Depended On By**

- `api.cpp`
- `dns_server.cpp`
- `main.cpp`

---

`wifi_manager.h`  
`wifi_manager.cpp`

The WiFi manager module is responsible for connecting the ESP32 to the configured WiFi network during startup.

`Main Public Functions`

- `connectWiFi()` - Connects to WiFi using the credentials from `secrets.h` and waits until a connection is established

**Depends On**

- ESP32 WiFi
- `secrets.h`

**Depended On By**

- `main.cpp`
