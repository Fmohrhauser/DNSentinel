#ifndef DNS_SERVER_H
#define DNS_SERVER_H

#include <Arduino.h>
extern bool upstreamOnline;
extern unsigned long lastUpstreamSuccess;
extern unsigned long totalUpstreamLatency;
extern unsigned long upstreamRequests;
extern unsigned long upstreamFailures;



void startDNSServer();
void handleDNS();

#endif
