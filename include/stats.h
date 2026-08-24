#ifndef STATS_H
#define STATS_H

#include <Arduino.h>


String createStatsJSON();

void incrementTotalRequests();
void incrementBlockedRequests();
void incrementForwardedRequests();

float getBlockedPercent();
float getForwardedPercent();
float getCachePercent();

float getSuccessRate();

unsigned long getQueriesPerMinute();


#endif