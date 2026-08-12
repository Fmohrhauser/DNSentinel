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
    for(int i = 0; i< CACHE_SIZE; i++)
    {
        if (!cache[i].valid)
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
            unsigned long ttl = getDNSResponseTTL(
                response,
                responseLength
            );

            if(ttl == 0)
            {

                return;
            }
            cache[i].expiresAt = millis() + (ttl * 1000);

            return;
        }
        
    }

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
            recordCacheHit(lookupTime);
            memcpy(
                response,
                cache[i].response,
                cache[i].responseLength
             );

            responseLength = cache[i].responseLength;
             incrementCacheHits();
             lookupTime = millis() - lookupTime;
            return true;
        }
    }
    recordCacheMiss(lookupTime);
    lookupTime = millis() - lookupTime;
    return false;
}


void cacheCleanup()
    {
        for(int i=0; i< CACHE_SIZE; i++)
        {
            if(cache[i].valid && millis() > cache[i].expiresAt)
            {
                cache[i].valid = false;


                cache[i].domain = "";
            }
        }
    }