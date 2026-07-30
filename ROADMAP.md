# DNSentinel Roadmap

☐ = TODO  
/ = In progress  
- = Complete  

---

# Version 0.1 — Core DNS Sinkhole

- Working DNS sinkhole
- DNS request parsing
- Domain blocking
- DNS response generation
- Upstream DNS forwarding

---

# Version 0.2 — Project Structure

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

☐ Documentation

---

# Version 0.3 — DNS Features

- DNS caching
- Request statistics
- Query logging
- A record support
- AAAA record support

☐ Larger blocklists
☐ Wildcard blocklist support
☐ Local whitelist

---

# Version 0.4 — Web Dashboard

- Web dashboard
- LittleFS frontend hosting
- Live statistics API
- Recent query monitoring
- System information display
- Cache hit monitoring
- Request breakdown visualization
- Top blocked domain analytics

/ Dashboard UI improvements

☐ Dashboard settings
☐ Web configuration controls
☐ Persistent configuration storage

---

# Version 1.0 — Stable Release

☐ Complete documentation
☐ Setup instructions
☐ Improve error handling
☐ Performance testing
☐ User testing
☐ Stable firmware release

---

# Version 2.0 — Hardware Expansion

☐ ESP32-P4 port
☐ ESP32-S31 port
☐ Ethernet support
☐ Custom PCB design
☐ OLED status display