# DNSentinel

An ESP32-based DNS sinkhole designed to block unwanted domains at the network level while providing local monitoring and configuration through a responsive web dashboard.

## Current Features

### DNS

- DNS packet parsing
- A and AAAA record support
- Domain blocking
- Multiple blocking modes
- DNS response generation
- Upstream DNS forwarding
- Configurable upstream DNS server
- DNS caching
- Cache hit tracking
- Query logging
- Request statistics
- Top blocked domain analytics
- Upstream DNS health monitoring

### Web Dashboard

- Web dashboard hosted from LittleFS
- Responsive desktop and mobile layouts
- Live DNS statistics
- Request breakdown visualization
- Recent DNS query monitoring
- Top blocked domain analytics
- Cache hit statistics
- System information
- Query searching and filtering
- Query sorting
- Configurable query display limits
- Query-log clearing
- Active-page sidebar highlighting
- Blocklist management
- Settings management
  
### Configuration

- Persistant settings storage
- Restore default settings
- Configurable upstream DNS
- Configurable blocking mode
- Configurable redirect IP
- Enable/disable blocking
- Enable/disable caching
- Enable/disable query logging
  
### API

- JSON APIs for dashboard data
- API input validation
- Consistent JSON error/status responses
- HTTP status-code handling
- Query limit validation
- Reusable Get route registration
- Protected management endpoints

### Authentication

- First time authentication setup
- Username and password authentication
- Password hashing
- Persistent authentication settings
- HTTP Basic Authentication
- Protected sensitive API endpoints
- Authentication state separated from normal settings reset

## Hardware

Current development hardware:

- ESP32-S3-WROOM-1 N16R8

Future hardware targets include:

- ESP32-P4
- Ethernet-capable hardware
- Custom PCB
- Optional OLED status display

## Architecture

DNSentinel separates DNS processing, the web API, and the browser frontend into independent modules.

### Firmware

The ESP32 firmware handles:

- DNS processing
- Blocking
- DNS caching
- Query logging
- Statistics
- Blocklists
- Settings
- Authentication
- System monitoring
- Web API behavior

### Frontend

LittleFS stores:

- HTML
- CSS
- JavaScript

The frontend communicates with the firmware through JSON APIs.

### Current Web Stack

DNSentinel currently uses the Arduino `WebServer` implementation.

The next major architecture change is migrating the HTTP layer to ESP-IDF's HTTP server. This will provide a better foundation for the ESP32-P4 version and future security features such as HTTPS.

## Current Development

The current development focus is:

1. Migrate the HTTP API from Arduino `WebServer` to ESP-IDF HTTP server
2. Preserve the existing API behavior during the migration
3. Create a formal DNSentinel security threat model
4. Research and implement appropriate local-network security protections
5. Add request and import size limits
6. Add protection against request flooding and authentication abuse
7. Continue performance and stress testing
8. Complete documentation and setup instructions

## Planned Features

### DNS

- Wildcard blocklist support
- Local whitelist
- Pi-hole-compatible blocklists
- Additional DNS record handling where useful

### Security

- Formal security threat model
- ESP-IDF HTTP server migration
- HTTPS/TLS investigation and implementation
- Authentication rate limiting
- Stronger password-storage design
- Password changing/recovery flow
- Request-size limits
- Blocklist import-size limits
- Request flooding protection
- Malformed-request testing
- Security event logging
- Review of stored credentials and secrets

### Performance

- Memory profiling
- PSRAM optimization
- DNS request stress testing
- Response latency testing
- Memory optimization

### Future

- Export/import configuration
- OTA updates
- ESP32-P4 Ethernet version
- Custom PCB
- OLED status display
- Stable version 1.0 release
