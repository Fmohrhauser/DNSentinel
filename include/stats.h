#ifndef STATS_H
#define STATS_H

#include <Arduino.h>

extern unsigned long totalRequests;
extern unsigned long blockedRequests;
extern unsigned long forwardedRequests;

int getTotalQueries();

int getBlockedQueries();

int getForwardedQueries();


int getCacheHits();
void incrementCacheHits();
void incrementTotalQueries();
void incrementBlockedQueries();
void incrementForwardedQueries();

String createStatsJSON();
String createQueryStatsJSON();

#endif