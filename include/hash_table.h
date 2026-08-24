#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <Arduino.h>

class DomainHashTable
{
public:
    DomainHashTable();

    bool begin(
        size_t tableSize,
        size_t poolSize
    );

    bool add(String domain);
    bool contains(String domain);
    bool remove(String domain);

    void clear();

    int size();

    String getSlot(size_t slot);
    size_t getSlotCount();

    size_t getTableCapacity();
    size_t getPoolCapacity();
    size_t getPoolUsed();

private:

    enum EntryState : uint8_t
    {
        EMPTY,
        OCCUPIED,
        DELETED
    };

    struct Entry
    {
        uint32_t hash;
        uint32_t domainOffset;
        uint8_t state;
    };

    Entry* table;
    char* domainPool;

    size_t tableSize;
    size_t poolSize;
    size_t poolUsed;

    int entryCount;

    uint32_t hash(const String& domain);

};

#endif