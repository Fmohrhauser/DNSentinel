#include "query_log.h"
#include "debug.h"



QueryLogEntry logs[MAX_QUERY_LOGS];

int logIndex = 0;

int queryCount = 0;

void initQueryLog()
{
    logIndex = 0;
    queryCount = 0;
}

void logQuery(
    const String& domain,
    QueryAction action
)
{
    DEBUG_PRINT(millis());
    DEBUG_PRINT("ms | ");


    
    
     logs[logIndex].domain = domain;
     logs[logIndex].action = action;
     logs[logIndex].timestamp = millis();

     logIndex++;

     if(logIndex >= MAX_QUERY_LOGS)
     {
         logIndex = 0;
     }


     if(queryCount < MAX_QUERY_LOGS)
    {
        queryCount++;
     }


    
}

int getQueryCount()
{
    return queryCount;
}

QueryLogEntry getQuery(
    int index
)
{
    return logs[index];
}
//debug stuff
void printLogs()
{
    DEBUG_PRINTLN("---- Query Logs ----");

    for(int i = 0; i < getQueryCount(); i++)
    {
        DEBUG_PRINT(logs[i].timestamp);
        DEBUG_PRINT("ms | ");

        DEBUG_PRINT(logs[i].domain);
        DEBUG_PRINT(" | ");

        switch(logs[i].action)
        {
            case BLOCKED:
                DEBUG_PRINTLN("BLOCKED");
                break;
            case CACHE_HIT:
                DEBUG_PRINTLN("CACHE HIT");
                break;
            case FORWARDED:
                DEBUG_PRINTLN("FORWARDED");
                break;
        }


    }
}