# DNSentinel TODO
☐ = TODO
/ = partly finished(working on)
- = done
Core
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

Cleanup
☐ Add project comments/documentation
☐ Create README with setup instructions

DNS Features
- Response cache
- Configurable upstream DNS server
- Wildcard blocklist support
☐ Import larger blocklists
☐ Local whitelist
- Query logging

Dashboard
☐ Basic web server
☐ Live statistics page
☐ Blocked vs forwarded graph
☐ Current uptime
☐ Top blocked domains
☐ Settings page

Performance
☐ Memory profiling
☐ PSRAM cache optimization
☐ Stress test with many DNS requests
☐ Measure response latency

Future Hardware
☐ Port project to ESP32-P4
☐ Port project to ESP32-S31
☐ Design custom PCB
☐ Add Ethernet support
☐ Add OLED status display

Known Bugs
☐ no ipv6 support (semi-bug, it adds a couple problems)


Ideas
☐ DNS-over-HTTPS support
☐ DNS-over-TLS
☐ OTA updates
☐ Multiple user profiles
☐ Pi-hole compatible blocklists
☐ Web configuration wizard

DNS Record Types
- A
- AAAA
☐ CNAME
☐ MX
☐ TXT
☐ SRV