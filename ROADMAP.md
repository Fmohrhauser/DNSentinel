# DNSentinel Roadmap

☐ = TODO  
/ = In progress  
- = Complete  

---

# Version 0.1 - Core DNS Sinkhole

- Working DNS sinkhole
- DNS request parsing
- Domain blocking
- DNS response generation
- Upstream DNS forwarding

---

# Version 0.2 - Project Structure

- Migrate to PlatformIO
- Configure ESP32-S3 N16R8
- Verify PSRAM
- Move project to GitHub
- Modularize parser
- Modularize blocklist
- Modularize WiFi
- Compile-time debug logging
- Finished dns_server cleanup
- Reduce unnecessary Serial output
- Documentation

---

# Version 0.3 - DNS Features

- DNS caching
- Request statistics
- Query logging
- A record support
- AAAA record support
- Larger blocklists
- Upstream DNS health monitoring
- Local whitelist

---

# Version 0.4 - Web Dashboard

- Web dashboard
- LittleFS frontend hosting
- Live statistics API
- Recent query monitoring
- System information display
- Cache hit monitoring
- Request breakdown visualization
- Top blocked domain analytics
- Dashboard settings
- Web configuration controls
- Persistent configuration storage
- Configuration API
- Blocklist management

- DNS health monitoring
- Dashboard redesign
- Larger blocklist importing
- Queries page redesign
- Mobile responsive layout
- Active-page sidebar highlighting
- Query search and filtering controls
- Query sorting controls
- Configurable query display limit
- Clear query log control
- Responsive query table
- Blocklist page redesign
- Settings page redesign
- Shared frontend styling structure
- Whitelist management

---

# Version 0.5 - API and Authentication

- Authentication backend
- First-time authentication setup
- Password hashing
- Persistent authentication configuration
- Protect sensitive API endpoints
- Separate authentication reset behavior from normal settings reset
- Validate API JSON input
- Validate blocking mode
- Validate upstream DNS
- Validate redirect IP
- Validate query limits
- Improve HTTP status codes
- Standardize JSON error responses
- Standardize JSON status responses
- Remove obsolete API debug output
- Add reusable GET route registration
- Refactor repeated API response handling

---

# Version 0.6 - ESP-IDF Web Migration

- Research ESP-IDF HTTP server architecture

- Migrate from Arduino `WebServer` to ESP-IDF `esp_http_server`
- Port basic GET route registration
- Port authenticated GET routes'
- Port POST request body handling
- Port LittleFS static file hosting
- Verify dashboard compatibility
- Remove Arduino `WebServer` dependency
- Regression test all API endpoints

- Research ESP-IDF HTTPS server migration

---

# Version 0.7 - Security Hardening

- Create formal security threat model
- Define DNSentinel security guarentees
- Review local-network attack surface
- Review authentication design
- Research HTTPS/TLS deployment
- Add authentication rate limiting/backoff
- Review password hashing and salting
- Add authenticated password-change flow
- Add request-size limits
- Add blocklist import-size limits
- Validate DNS packet lengths
- Review memory and buffer limits
- Test malformed HTTP requests
- Test malformed DNS packets
- Test request flooding behavior
- Review storage of WiFi credentials and secrets
- Add security-ralated logging where appropriate

---

# Version 0.8 - Performance and Reliability

- Memory profiling
- DNS request stress testing
- HTTP request stress testing
- Measure DNS response latency
- Measure web API latency
- Add cache eviction behavior
- Fix blocked stats buffer overflow
- Test upstream DNS failure and recovery
- PSRAM optimization
- Large-blocklist testing
- Long-term uptime testing
- Memory optimization


---

# Version 1.0 - Stable Release

- Complete project README
- Add setup and installation instructions
- Consolidate architecture documentation into the README
- Remove unused and duplicate code
- Clean up debug and development leftovers
- Validate runtime settings behavior
- Improve startup reliability
- Harden query-log JSON output
- Review memory-management behavior
- Final API cleanup
- Final frontend cleanup
- Final codebase scrub
- Clean production build
- Stable firmware release

---

# Version 1.x - Post-release Improvments

☐ Test Wi-Fi disconnect/reconnect recovery
☐ Expand malformed DNS packet testing
☐ Expand malformed HTTP request testing
☐ Gather more detailed performance benchmarks
☐ Improve large-list API/frontend behavior
☐ Configuration export/import
☐ OTA firmware updates
☐ Additional DNS protocol support
☐ Additional memory profiling and optimization
☐ Additional dashboard UX improvements
☐ Additional network-security hardening
☐ HTTPS/TLS management interface

# Version 2.0 — Hardware Expansion

☐ ESP32-P4 port
☐ ESP32-S31 port
☐ Ethernet support
☐ Custom PCB design
☐ OLED status display
☐ Secure firmware/update architecture