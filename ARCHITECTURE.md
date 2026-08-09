# DNSentinel Architecture

This document describes the main DNSentinel modules, what each module is responsible for, and how they interact.

---

## Startup Flow

1. Initialize hardware and serial output
2. Initialize default settings
3. Initialize authentication defaults
4. Load persisted settings
5. Initialize Wi-Fi
6. Initialize storage and blocklist data
7. Initialize DNS services
8. Start the web API
9. Enter the main runtime loop

---

## Modules

