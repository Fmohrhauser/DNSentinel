#include "hash_table.h"
#include <esp_heap_caps.h>
#include <cstring>
#include "debug.h"

DomainHashTable::DomainHashTable()
{
    table = nullptr;
    domainPool = nullptr;

    tableSize = 0;
    poolSize = 0;
    poolUsed = 0;

    entryCount = 0;
}

bool DomainHashTable::begin(
    size_t requestedTableSize,
    size_t requestedPoolSize
)
{
    tableSize = requestedTableSize;
    poolSize = requestedPoolSize;

    table = (Entry*)heap_caps_calloc(
        tableSize,
        sizeof(Entry),
        MALLOC_CAP_SPIRAM
    );

    if(table == nullptr)
    {
        DEBUG_PRINTLN("HASH TABLE PSRAM ALLOCATION FAILED");
        return false;
    }

    domainPool = (char*)heap_caps_malloc(
        poolSize,
        MALLOC_CAP_SPIRAM
    );

    if(domainPool == nullptr)
    {
        DEBUG_PRINTLN("DOMAIN POOL PSRAM ALLOCATION FAILED");

        heap_caps_free(table);
        table = nullptr;

        return false;
    }

    DEBUG_PRINT("Hash table entry size: ");
    DEBUG_PRINTLN(sizeof(Entry));

    DEBUG_PRINT("Hash table bytes: ");
    DEBUG_PRINTLN(sizeof(Entry) * tableSize);

    DEBUG_PRINT("Domain pool bytes: ");
    DEBUG_PRINTLN(poolSize);

    clear();

    DEBUG_PRINTLN("HASH TABLE ALLOCATED");

    return true;
}

void DomainHashTable::clear()
{

    if(table == nullptr || domainPool == nullptr)
    {
        DEBUG_PRINTLN("Cannot clear null hash table");
        return;
    }

    entryCount = 0;
    poolUsed = 0;

    memset(table, 0, sizeof(Entry) * tableSize);

    DEBUG_PRINTLN("HASH CLEAR COMPLETE");
}

uint32_t DomainHashTable::hash(const String& domain)
{
    uint32_t hash = 2166136261UL;

    for(char c : domain)
    {
        hash ^= (uint8_t)c;
        hash *= 16777619UL;
    }

    return hash;
}

bool DomainHashTable::add(String domain)
{
    if(table == nullptr || domainPool == nullptr)
        return false;

    domain.toLowerCase();

    uint32_t h = hash(domain);

    size_t index = h % tableSize;

    int firstDeleted = -1;

    for(size_t i = 0; i < tableSize; i++)
    {
        size_t current =
            (index + i) % tableSize;

        if(table[current].state == DELETED)
        {
            if(firstDeleted == -1)
            {
                firstDeleted = current;
            }

            continue;
        }

        if(table[current].state == OCCUPIED)
        {
            const char* storedDomain =
                &domainPool[
                    table[current].domainOffset
                ];

            if(
                table[current].hash == h &&
                strcmp(
                    storedDomain,
                    domain.c_str()
                ) == 0
            )
            {
                return false;
            }

            continue;
        }

        if(table[current].state == EMPTY)
        {
            if(firstDeleted != -1)
            {
                current = firstDeleted;
            }

            size_t domainLength =
                domain.length() + 1;
            
            if(poolUsed + domainLength > poolSize)
            {
                DEBUG_PRINTLN("DOMAIN POOL FULL");
                return false;
            }

            uint32_t offset = poolUsed;

            memcpy(
                &domainPool[offset],
                domain.c_str(),
                domainLength
            );

            poolUsed += domainLength;

            table[current].hash = h;
            table[current].domainOffset = offset;
            table[current].state = OCCUPIED;

            entryCount ++;

            return true;
        }
    }

    if(firstDeleted != -1)
    {
        size_t domainLength =
            domain.length() + 1;

        if(poolUsed + domainLength > poolSize)
        {
            DEBUG_PRINTLN("DOMAIN POOL FULL");
            return false;
        }

        uint32_t offset = poolUsed;

        memcpy(
            &domainPool[offset],
            domain.c_str(),
            domainLength
        );

        poolUsed += domainLength;

        table[firstDeleted].hash = h;
        table[firstDeleted].domainOffset = offset;
        table[firstDeleted].state = OCCUPIED;

        entryCount++;

        return true;
    }

    DEBUG_PRINTLN("HASH TABLE FULL");

    return false;
}
// removeed entries leave their domain data in the PSRAM pool(its linear)
// pool space is reclaimed when the table is cleared or rebuilt
// This acoids expensive pool compaction during individual removals.
bool DomainHashTable::remove(String domain)
{
    if(table == nullptr || domainPool == nullptr)
        return false;

    domain.toLowerCase();

    uint32_t h = hash(domain);

    size_t index = h % tableSize;

    for(size_t i = 0; i < tableSize; i++)
    {
        size_t current =
            (index + i) % tableSize;

        if(table[current].state == EMPTY)
            return false;

        if(
            table[current].state == OCCUPIED &&
            table[current].hash == h
        )
        {
            const char* storedDomain =
                &domainPool[table[current].domainOffset];
            
            if(strcmp(
                storedDomain,
                domain.c_str()
            ) == 0)
            {
                table[current].state = DELETED;
                table[current].hash = 0;
                table[current].domainOffset = 0;

                entryCount--;

                return true;
            }
        }
    }

    return false;
}

bool DomainHashTable::contains(String domain)
{
    if(table == nullptr || domainPool == nullptr)
        return false;

    domain.toLowerCase();

    uint32_t h = hash(domain);

    size_t index = h % tableSize;

    for(size_t i = 0; i < tableSize; i++)
    {
        size_t current =
            (index + i) % tableSize;

        if(table[current].state == EMPTY)
            return false;

        if(
            table[current].state == OCCUPIED &&
            table[current].hash == h
        )
        {
            const char* storedDomain = &domainPool[table[current].domainOffset];

            if(strcmp(
                storedDomain,
                domain.c_str()
            ) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

int DomainHashTable::size()
{
    return entryCount;
}


String DomainHashTable::getSlot(size_t slot)
{
    if(table == nullptr|| domainPool == nullptr)
        return "";

    if(slot >= tableSize)
        return "";

    if(table[slot].state != OCCUPIED)
        return "";

    return String(&domainPool[table[slot].domainOffset]);
}

size_t DomainHashTable::getSlotCount()
{
    return tableSize;
}

size_t DomainHashTable::getPoolCapacity()
{
    return poolSize;
}

size_t DomainHashTable::getPoolUsed()
{
    return poolUsed;
}

