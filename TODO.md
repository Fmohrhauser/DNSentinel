# DNSentinel TODO

☐ = TODO
/ = In progress
- = Complete

---

# Core

- Migrate to PlatformIO
- Configure ESP32-S3 N16R8
- Verify PSRAM
- Move project to GitHub
- DNS request parsing
- DNS blocking
- DNS forwarding
- Statistics tracking
- Modularize parser
- Modularize blocklist
- Modularize WiFi
- Compile-time debug logging
- Finished dns_server cleanup
- Reduce unnecessary Serial output

☐ Add project comments/documentation
☐ Create README with setup instructions

---

# DNS Features

- DNS cache
- Configurable upstream DNS server
- A record support
- AAAA record support
- Query logging
- Cache hit tracking

☐ Import larger blocklists
☐ Wildcard blocklist support
☐ web blocklist management
☐ Local whitelist
☐ Upstream DNS health monitoring
☐ Measure upstream latency
☐ DNS server status API

---

# Dashboard

- Basic web server
- LittleFS webpage hosting
- Live statistics API
- Recent query log API
- System information API
- Live statistics page
- Recent queries display
- Request breakdown visualization
- Top blocked domains analytics
- Hide IP privacy toggle

/ Dashboard UI improvements
- settings popup panel
- Web configuration controls
- Persistent settings storage
- Configuration API
- Save settings button
- Restore defaults button


☐ Blocklist management interface
☐ Dashboard CSS redesign
☐ mobile responsive layout
☐ Dashboard Charts
☐ Tabs
☐ Configurable "Home Screen"


---

# Performance

☐ Memory profiling
☐ PSRAM cache optimization
☐ Stress test with many DNS requests
☐ Measure response latency
☐ Optimize memory usage

---

# Future Hardware

☐ Port project to ESP32-P4
☐ Port project to ESP32-S31
☐ Design custom PCB
☐ Add Ethernet support
☐ Add OLED status display

---

# Known Bugs

☐ IPv4 + IPv6 requests create duplicate statistics entries
    (Technically correct DNS behavior, but may confuse users)

---

# Ideas

☐ DNS-over-HTTPS support
☐ DNS-over-TLS
☐ OTA updates
☐ Multiple user profiles
☐ Pi-hole compatible blocklists
☐ Web configuration wizard

---

# DNS Record Types

- A
- AAAA

☐ CNAME
☐ MX
☐ TXT
☐ SRV

---

# Debug Type Settings

- Overall debug serial print

☐ Specific debug serial prints