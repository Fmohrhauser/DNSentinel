#include "query_log.h"
#include "debug.h"
#include "time_manager.h"
#include "stats.h"



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



    
    
     logs[logIndex].domain = domain;
     logs[logIndex].action = action;
     logs[logIndex].timestamp = getCurrentTime();

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

void getLatestQueries(
    QueryLogEntry output[],
    int amount
)
{
    if(amount > queryCount)
    {
        amount = queryCount;
    }

    for(int i = 0; i < amount; i++)
    {

        int index = logIndex - 1 - i;

        if(index < 0)
        {
            index += MAX_QUERY_LOGS;
        }


        output[i] = logs[index];
    }
}


String actionToString(QueryAction action)
{
    switch(action)
    {
        case BLOCKED:
            return "BLOCKED";
        case CACHE_HIT:
            return "CACHE HIT";
        case FORWARDED:
            return "FORWARDED";
    }

    return "UNKNOWN";
}

String createQueryLogJSON(int limit)
{
    String json = "[";

    QueryLogEntry recent[MAX_QUERY_LOGS];

    getLatestQueries(recent, MAX_QUERY_LOGS);

    if(limit > queryCount)
    {
        limit = queryCount;
    }

    for(int i = 0; i < limit; i++)
    {
        if(i !=0)
        {
            json +=",";
        }
       json += "{";

       json += "\"domain\":\"";
       json += recent[i].domain;
       json += "\",";

       json += "\"action\":\"";
       json += actionToString(recent[i].action);
       json += "\"";

       json += ",";

       json += "\"timestamp\":\"";
       json += recent[i].timestamp;
       json += "\"";

       json += "}";

        
    }
    json += "]";

    return json;
}

void clearLogs()
{
    logIndex = 0;
    queryCount = 0;

    for(int i = 0; i < MAX_QUERY_LOGS; i++)
    {
        logs[i].domain = "";
        logs[i].action = FORWARDED;
        logs[i].timestamp = "";
    }

    DEBUG_PRINTLN("Query log cleared");
}




