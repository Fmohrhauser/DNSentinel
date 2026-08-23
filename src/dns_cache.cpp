#include "dns_cache.h"
#include "debug.h"
#include "dns_parser.h"
#include "stats.h"
#include "cache_stats.h"


CacheEntry cache[CACHE_SIZE];


void initCache()
{
    for(int i = 0; i< CACHE_SIZE; i++)
    {
        cache[i].valid = false;
    }
}

void cacheInsert(
    const String& domain,
    uint16_t qType,
    const byte response[],
    int responseLength
)
{
    unsigned long ttl =
        getDNSResponseTTL(
            response,
            responseLength
        );

    if(ttl == 0)
    {
        return;
    }

    for(int i = 0; i< CACHE_SIZE; i++)
    {
        if (!cache[i].valid || millis() > cache[i].expiresAt)
        {
            cache[i].valid = true;

            cache[i].domain = domain;
            cache[i].qType = qType;
            memcpy(
                cache[i].response,
                response,
                min(responseLength, (int)MAX_DNS_PACKET_SIZE)
            );
            cache[i].responseLength = min(
                responseLength,
                (int)MAX_DNS_PACKET_SIZE
            );
            cache[i].expiresAt = millis() + (ttl * 1000);
            cache[i].insertedAt = millis();

            return;
        }
        
    }

    int oldestIndex = 0;

    for(int i = 1; i < CACHE_SIZE; i++)
    {
        if(cache[i].insertedAt < cache[oldestIndex].insertedAt)
        {
            oldestIndex = i;
        }
    }
    cache[oldestIndex].valid = true;
    cache[oldestIndex].domain = domain;
    cache[oldestIndex].qType = qType;

    memcpy(
        cache[oldestIndex].response,
        response,
        min(responseLength, (int)MAX_DNS_PACKET_SIZE)
    );

    cache[oldestIndex].responseLength =
        min(
            responseLength,
            (int)MAX_DNS_PACKET_SIZE
        );

    cache[oldestIndex].expiresAt =
        millis() + (ttl * 1000);

    cache[oldestIndex].insertedAt =
        millis();

}

bool cacheLookup(
    const String& domain,
    uint16_t qType,
    byte response[],
    int& responseLength
)
{

    unsigned long lookupTime = millis();
    for(int i=0; i< CACHE_SIZE; i++)
    {
        if(!cache[i].valid)
        {
            continue;
        }

        if(millis() > cache[i].expiresAt)
        {
            cache[i].valid = false;
            continue;
        }

        if(cache[i].domain == domain &&
           cache[i].qType == qType 
        )
        {
            unsigned long elapsedTime = millis() - lookupTime;
            recordCacheHit(elapsedTime);
            memcpy(
                response,
                cache[i].response,
                cache[i].responseLength
             );

            responseLength = cache[i].responseLength;
            return true;
        }
    }
    unsigned long elapsedTime = millis() - lookupTime;

    recordCacheMiss(elapsedTime);

    return false;
}
