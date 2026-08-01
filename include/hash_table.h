#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <Arduino.h>

class DomainHashTable
{
public:
    DomainHashTable();

    void begin();

    bool add(String domain);
    bool contains(String domain);
    bool remove(String domain);

    void clear();

    int size();

    String get(int index);

private:

    struct Entry
    {
        uint32_t hash;
        char domain[128];
        enum State
        {
            EMPTY,
            OCCUPIED,
            DELETED
        };

        State state;
    };

    static const int TABLE_SIZE = 4096;

    Entry* table;

    int entryCount;

    uint32_t hash(const String& domain);

    int findIndex(const String& domain);
};

#endif