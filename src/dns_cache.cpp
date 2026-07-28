#include "dns_cache.h"
#include "debug.h"


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
            memcpy(
                cache[i].response,
                response,
                min(responseLength, (int)MAX_DNS_PACKET_SIZE)
            );
            cache[i].responseLength = min(
                responseLength,
                (int)MAX_DNS_PACKET_SIZE
            );

            cache[i].expiresAt = millis() + 60000;
            DEBUG_PRINT("Cached: ");
            DEBUG_PRINTLN(domain);

            return;
        }
        
    }

}

bool cacheLookup(
    const String& domain,
    byte response[],
    int& responseLength
)
{
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

        if(cache[i].domain == domain)
        {
            memcpy(
                response,
                cache[i].response,
                cache[i].responseLength
             );

            responseLength = cache[i].responseLength;

            DEBUG_PRINT("Cache hit: ");
            DEBUG_PRINTLN(domain);

            return true;
        }
    }

    DEBUG_PRINT("Cache miss: ");
    DEBUG_PRINTLN(domain);

    return false;
}


void cacheCleanup()
    {
        for(int i=0; i< CACHE_SIZE; i++)
        {
            if(cache[i].valid && millis() > cache[i].expiresAt)
            {
                cache[i].valid = false;

                DEBUG_PRINT("Removed expired cache entry: ");
                DEBUG_PRINTLN(cache[i].domain);

                cache[i].domain = "";
            }
        }
    }