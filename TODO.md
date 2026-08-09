# DNSentinel TODO

This file tracks what I should work on next.

For long-term versions and project direction, see `ROADMAP.md`.

---

# Current Goal - Finish version 0.4

Complete the remaining Version 0.2, 0.3, and 0.4 work so DNSentinel can officially move into Version 0.5.

---

## 1. Finish Version 0.2 - Project Structure

### Documentation

- [] Add useful comments to major modules
- [] Document important public functions in header files
- [] Document module responsibilities
- [] Document initialization order
- [] Document important dependencies between modules
- [] Clean up outdated comments

### Version 0.2 Complete When

- [] Core project structure is documented well enough to understand after returning to the project later
- [] README accurately describes the project
- [] Major modules and APIs are understandable without reading every implementation file
  
---

## 2. Finish Version 0.3 - DNS Features

### Wildcard Blocking Backend

- [] Decide wildcard syntax
- [] decide how wildcard domains will be stored
- [] Determine how wildcard matching fits with `DomainHashTable`
- [] Implement wildcard domain matching
- [] Make wildcard matching work with normal blocklist lookups
- [] Test exact-domain vs wildcard behavior
- [] Test subdomains
- [] Test malformed wildcard entries
- [] Test wildcard performance with larger blocklists

### Local Whitelist Backend

- [] Decide whitelist data structure
- [] Add whitelist module or extend blocklist architecture cleanly
- [] Add domain to whitelist
- [] Remove domain from whitelist
- [] Check whitelist before blocklist
- [] Decide how whitelist interacts with wildcard blocking
- [] Persist whitelist data
- [] Load whitelist on boot
- [] Test whitelist overide behavior
- [] Test exact and wildcard whitelist behavior if wildcard whitelisting is supported

### Version 0.3 Complete When

- [] Wildcard blocking works reliably
- [] Local whitelist works reliably
- [] Whitelist correctly overrides blocked domains
- [] Both features survive reboot
- [] Existing DNS behavior still works
  
---

## 3. Finish Version 0.4 - Web Dashboard

### Wildcard Management
- [] Allow wildcard entries to be added through the Blocklist page
- [] Display wildcard entries clearly
- [] Allow wildcard entries to be removed
- [] Validate wildcard input in the API
- [] Display useful errors for invalid wildcard entries

### Whitelist Management

- [] Add whitelist API endpoints
- [] Add whitelist section/page to the frontend
- [] Display whitelist entries
- [] Add domains to whitelist
- [] Remove domains from whitelist
- [] Add input validation
- [] Add frontend success/error feedback
- [] Test mobile layout

### Frontend Final Testing

- [] Test Dashboard on device
- [] Test Queries page on device
- [] Test Blocklist page on device
- [] Test settings page on device
- [] Test movile layout
- [] Test navigation
- [] Test settings changes
- [] Test blocklist changes
- [] Test query controls
- [] Test authentication with frontend requests
- [] Fix any remaining frontend bugs

### Version 0.4 Complete When

- [] Dashboard features work on the actual ESP32
- [] Blocklist management is complete
- [] Wildcards can be managed from the dashboard
- [] Whitelist can be manaed from the dashboard
- [] All major frontend pages work on desktop and mobile
- [] No known major frontend bugs remain

---

# Then - Version 0.5 API and Authentication

Some Version 0.5 work has already been completed early.

## Already Completed

- [x] Authentication backend
- [x] First-time authentication setup
- [x] Password hashing
- [x] Persistant authentication configuration
- [x] Protect sensitive API endpoints
- [x] Separate authentication from normal settings reset
- [x] Validate API JSON input
- [x] Validate blocking mode
- [x] Validate upstream DNS
- [x] Validate redirect IP
- [x] Validate query limits
- [x] Improve HTTP status codes
- [x] Standardize simple JSON error responses
- [x] Standardize simple JSON status responses
- [x] Remove obsolete API debugging output
- [x] Add reusable GET route registration
- [x] Refactor repeated API response handling

## Remaining Version 0.5 Work

### ESP-IDF HTTP Server Migration

- [] Learn basic `esp_http_server` architecture
- [] Create an ESP-IDF HTTP server prototype
- [] Port `/api/stats`
- [] Port reusable Get endpoints/system
- [] Port POST request parsing
- [] Port JSON response helpers
- [] Port authentication
- [] Port LittleFS static file hosting
- [] Test every API endpoint
- [] Test every frontend page
- [] Remove Arduino `WebServer`

### Version 0.5 Cleanup

- [] Document API endpoints
- [] Document which endpoints require authentication
- [] Regression test authentication
- [] Regression test settings persistence
- [] Regression test blocklist persistence
- [] Check RAM usage after IDF migration

### Version 0.5 Complete When

- [] Arduino `WebServer` is no longer used
- [] ESP-IDF HTTP server runs all dashboard/API traffic
- [] Authentication still works
- [] All exisiting API behavior still works
- [] Frontend works without changes in behavior
- [] API is documented
- [] No major regressions remain

---

# After Version 0.5

Version 0.6+ work should follow `ROADMAP.md`.

Major upcoming areas include:

- Security threat modeling
- HTTPS/TLS
- Authentication hardening
- Request-size limits
- Request-flooding protection
- DNS parser hardeing
- Performance testing
- ESP32-p4 migration
