#include "hash_table.h"
#include <esp_heap_caps.h>
#include <cstring>
#include "debug.h"

DomainHashTable::DomainHashTable()
{
    table = nullptr;
    entryCount = 0;
}

void DomainHashTable::begin()
{
    DEBUG_PRINTLN(sizeof(Entry));
    DEBUG_PRINTLN(sizeof(Entry) * TABLE_SIZE);

    table = (Entry*)heap_caps_calloc(
        TABLE_SIZE,
        sizeof(Entry),
        MALLOC_CAP_SPIRAM
    );

    if(table == nullptr)
    {
        DEBUG_PRINTLN("HASH TABLE PSRAM ALLOCATION FAILED");
        return;
    }

    Serial.println("HASH TABLE ALLOCATED");

    clear();
    
}

void DomainHashTable::clear()
{

    if(table == nullptr)
    {
        DEBUG_PRINTLN("Cannot clear null hash table");
        return;
    }

    entryCount = 0;

    for(int i = 0; i < TABLE_SIZE; i++)
    {

        table[i].state = Entry::EMPTY;
        table[i].hash = 0;
        memset(table[i].domain, 0, sizeof(table[i].domain));
    }

    Serial.println("HASH CLEAR COMPLETE");
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
    if(table == nullptr)
        return false;

    domain.toLowerCase();

    uint32_t h = hash(domain);

    int index = h % TABLE_SIZE;
    int firstDeleted = - 1;
    for(int i = 0; i < TABLE_SIZE; i++)
    {
        int current = (index + i) % TABLE_SIZE;

        if(table[current].state == Entry::DELETED)
        {
            if(firstDeleted == -1)
            {
                firstDeleted = current;
            }

            continue;
        }

        if(table[current].state == Entry::EMPTY)
        {
            if(firstDeleted != -1)
            {
                current = firstDeleted;
            }
            table[current].hash = h;

            strncpy(
                table[current].domain,
                domain.c_str(),
                sizeof(table[current].domain) - 1
            );

            table[current].domain[
                sizeof(table[current].domain) - 1
            ] = '\0';

            table[current].state = Entry::OCCUPIED;
            entryCount++;

            return true;
        }

        if(
            table[current].state == Entry::OCCUPIED &&
            table[current].hash == h &&
            strcmp(table[current].domain, domain.c_str()) == 0
        )
        {
            return false;
        }
    }

    return false;
}

bool DomainHashTable::remove(String domain)
{
    if(table == nullptr)
        return false;

    domain.toLowerCase();

    uint32_t h = hash(domain);

    int index = h % TABLE_SIZE;

    for(int i = 0; i < TABLE_SIZE; i++)
    {
        int current = (index + i) % TABLE_SIZE;

        if(table[current].state == Entry::EMPTY)
            return false;

        if(
            table[current].state == Entry::OCCUPIED &&
            table[current].hash == h &&
            strcmp(table[current].domain, domain.c_str()) == 0
        )
        {
            table[current].state = Entry::DELETED;
            table[current].domain[0] = '\0';
            table[current].hash = 0;
            entryCount--;

            return true;
        }
    }

    return false;
}

bool DomainHashTable::contains(String domain)
{
    if(table == nullptr)
        return false;

    domain.toLowerCase();

    uint32_t h = hash(domain);

    int index = h % TABLE_SIZE;

    for(int i = 0; i < TABLE_SIZE; i++)
    {
        int current = (index + i) % TABLE_SIZE;

        if(table[current].state == Entry::EMPTY)
            return false;

        if(
            table[current].state == Entry::OCCUPIED &&
            table[current].hash == h &&
            strcmp(table[current].domain, domain.c_str()) == 0
        )
        {
            return true;
        }
    }

    return false;
}

int DomainHashTable::size()
{
    return entryCount;
}

String DomainHashTable::get(int index)
{
    if(table == nullptr)
        return "";

    int found = 0;

    for(int i = 0; i < TABLE_SIZE; i++)
    {
        if(table[i].state == Entry::OCCUPIED)
        {
            if(found == index)
            {
                return String(table[i].domain);
            }

            found++;
        }
    }

    return "";
}