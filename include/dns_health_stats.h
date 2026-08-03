#ifndef DNS_HEALTH_STATS_H
#define DNS_HEALTH_STATS_H

extern unsigned long upstreamRequests;
extern unsigned long upstreamFailures;
extern unsigned long totalUpstreamLatency;

extern bool upstreamOnline;
extern bool upstreamChecked;
extern unsigned long lastUpstreamSuccess;

float getUpstreamSuccessRate();

#endif