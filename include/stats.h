#ifndef STATS_H
#define STATS_H

#include <Arduino.h>




int getCacheHits();
void incrementCacheHits();
String createStatsJSON();

void incrementTotalRequests();
void incrementBlockedRequests();
void incrementForwardedRequests();
void incrementCacheHits();

float getBlockedPercent();
float getForwardedPercent();
float getCachePercent();

float getSuccessRate();

unsigned long getQueriesPerMinute();


float getSuccessRate();

#endif