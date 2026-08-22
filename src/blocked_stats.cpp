#include "blocked_stats.h"

struct BlockedDomainStat
{
    String domain;
    int count;
};

BlockedDomainStat blockedStats[100];

int blockedStatCount = 0;

void incrementBlockedDomain(const String& domain)
{
    for(int i = 0; i < blockedStatCount; i++)
    {
        if(blockedStats[i].domain == domain)
        {
            blockedStats[i].count++;

            return;
        }
    }
    if(blockedStatCount >= 100)
    {
        return;
    }
    blockedStats[blockedStatCount].domain = domain;

    blockedStats[blockedStatCount].count = 1;

    blockedStatCount ++;

}

void sortBlockedStats()
{
    for(int i = 0;i < blockedStatCount - 1; i++)
    {
        for(int j = 0; j < blockedStatCount - i -1; j++)
        {
            if(blockedStats[j].count < blockedStats[j+1].count)
            {
                BlockedDomainStat temp = blockedStats[j];

                blockedStats[j] = blockedStats[j+1];

                blockedStats[j+1] = temp;
            }
        }
    }

}

String createTopBlockedJSON()
{
    sortBlockedStats();
    String json = "[";

    for(int i = 0; i < blockedStatCount; i++)
    {
        json += "{";

        json += "\"domain\":\"";
        json += blockedStats[i].domain;
        json += "\",";

        json += "\"count\":";
        json += String(blockedStats[i].count);

        json += "}";

        if(i < blockedStatCount -1)
        {
            json += ",";
        }
    }
    json += "]";
    return json;
}