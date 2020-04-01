/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Adriane Hairston
 * Date: February 27, 2020
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    for (int i = 0; i < map->capacity; i++)
    {
        struct HashLink * hlPtr = map->table[i];
        while(hlPtr != NULL){
            struct HashLink * garbage = hlPtr;
            hlPtr = hlPtr->next;
            hashLinkDelete(garbage);
        }
        
    }
    free(map->table);
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    assert (map != 0);
    
    int index = HASH_FUNCTION(key) % map->capacity;
    if (index < 0) index += map->capacity;
   
    int* linkVal = NULL;
    
    struct HashLink * hlPtr = map->table[index];
    while (hlPtr != NULL){
        if (strcmp(hlPtr->key, key) == 0){
            linkVal = &(hlPtr->value);
            return linkVal;
        }
        hlPtr = hlPtr->next;
    }
    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    struct HashLink ** oldTable = map->table;
    int oldSize = map->capacity;
    
    struct HashLink ** newTable = malloc(sizeof(HashLink*) * capacity);
    assert(newTable != 0);
    
    map->table = newTable;
    map->capacity = capacity;
    map->size = 0;
    
    for (int i = 0; i < capacity; i++){
        map->table[i] = NULL;
    }
    
    
    for(int i = 0; i < oldSize; i++){
      struct HashLink* temp = oldTable[i];
      while(temp != NULL){
              
          hashMapPut(map, temp->key, temp->value);
          struct HashLink* garbage = temp;
          temp = temp->next;
          hashLinkDelete(garbage);
      }
    }
    
    free(oldTable);
    oldTable = 0;
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    assert (map != 0);
       
    int index = HASH_FUNCTION(key) % map->capacity;
    if (index < 0) index += map->capacity;
    
    struct HashLink * hlPtr = map->table[index];
    
    while (hlPtr != 0){
        if (strcmp(hlPtr->key, key) == 0){
            hlPtr->value = value;
            return;
        }
        hlPtr = hlPtr->next;
    }
    hlPtr = map->table[index];
    
    if (hlPtr == NULL){
        hlPtr = hashLinkNew(key, value, NULL);
    }
    else{
        hlPtr = hashLinkNew(key, value, map->table[index]);
    }
    
    map->table[index] = hlPtr;
    
    hlPtr = 0;
    (map->size)++;
    
    if (hashMapSize(map)/(double)(hashMapCapacity(map)) >= MAX_TABLE_LOAD){
        resizeTable(map, (map->capacity)*2);
    }
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    assert(map != 0);
    /* Hash the key value to find which bucket to search*/
    int index = HASH_FUNCTION(key) % map->capacity;
    if (index < 0) index += map->capacity;
    
    /* Create a temp and previous link pointer. Prev will follow temp and
     connect temp's next to prev in the case that temp is removed.*/
    struct HashLink* temp = map->table[index];
    struct HashLink* prev = temp;
    if (temp != NULL && (hashMapContainsKey(map, key) == 1)){
        while (strcmp(temp->key, key) != 0){
            prev = temp;
            temp = temp->next;
        }
        /* If prev is at the same location as temp, they are at the head of the
         bucket. The index is set to temp's next, making it point to NULL*/
        if ( prev == temp){
            map->table[index] = temp->next;
        }
        /* Otherwise, the link being removed is not the head, and the gap is
         closed as planned.*/
        else{
            prev->next = temp->next;
        }
        /* The data for the link being removed is freed, the pointers are set
         to NULL, and the size of the map is decremented.*/
        hashLinkDelete(temp);
        temp = NULL;
        prev = NULL;
        (map->size)--;
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    assert(map != 0);
    
    int index = HASH_FUNCTION(key) % map->capacity;
    if (index < 0) index += map->capacity;
       
     struct HashLink* temp = map->table[index];
     while(temp != 0){
         if (strcmp(temp->key, key) == 0){
             return 1;
         }
       temp = temp->next;
     }
    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    int numEmptyBuckets = 0;
    int i;
    for (i = 0; i < map->capacity; i++){
        if (map->table[i] == NULL){
            numEmptyBuckets ++;
        }
    }
    
    return numEmptyBuckets;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    float loadFactor = map->size / (float) map->capacity;
    return loadFactor;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
    printf("\n");
    int i;
    for (i = 0; i < map->capacity; i++){
        struct HashLink * hlPtr = map->table[i];
        
        if (hlPtr == NULL || (strcmp(hlPtr->key, "") == 0)){
            printf("[Empty]");
        }
        while (hlPtr != 0){
            printf("[key: %s, value: %d] ", hlPtr->key, hlPtr->value);
            hlPtr = hlPtr->next;
        }
    
        printf("\n");
        
    }

   
}
