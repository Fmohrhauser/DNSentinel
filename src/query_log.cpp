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

void testLatest()
{
    QueryLogEntry recent[5];

    getLatestQueries(recent, 5);

    for(int i = 0; i < 5; i++)
    {
        DEBUG_PRINT(recent[i].domain);
        DEBUG_PRINT(" ");

        DEBUG_PRINTLN(actionToString(recent[i].action));
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

String createQueryLogJSON()
{
    String json = "[";

    QueryLogEntry recent[MAX_QUERY_LOGS];

    getLatestQueries(recent, MAX_QUERY_LOGS);

    for(int i = 0; i < queryCount; i++)
    {
       json += "{";

       json += "\"domain\":\"";
       json += recent[i].domain;
       json += "\",";

       json += "\"action\":\"";
       json += actionToString(recent[i].action);
       json += "\"";

       json += "}";

       if(i < queryCount - 1)
       {
        json += ",";
       }
        
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
        logs[i].timestamp = 0;
    }

    DEBUG_PRINTLN("Query log cleared");
}
