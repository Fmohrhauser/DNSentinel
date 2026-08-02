#ifndef QUERY_LOG_H
#define QUERY_LOG_H

#include <Arduino.h>

#define MAX_QUERY_LOGS 100

enum QueryAction {
    BLOCKED,
    CACHE_HIT,
    FORWARDED
};

struct QueryLogEntry {

    String domain;

    QueryAction action;

    unsigned long timestamp;
};

void initQueryLog();


void logQuery(
    const String& domain,
    QueryAction action
);


int getQueryCount();


QueryLogEntry getQuery(
    int index
);

void printLogs();

void getLatestQueries(
    QueryLogEntry output[],
    int amount
);

void testLatest();

String actionToString(QueryAction action);

String createQueryLogJSON(int limit);

void clearLogs();

#endif