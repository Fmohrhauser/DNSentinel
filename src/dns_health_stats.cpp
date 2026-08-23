#include "dns_health_stats.h"

unsigned long totalUpstreamLatency = 0;
unsigned long upstreamRequests = 0;
unsigned long upstreamFailures = 0;

bool upstreamOnline = false;
bool upstreamChecked = false;

unsigned long lastUpstreamSuccess = 0;
