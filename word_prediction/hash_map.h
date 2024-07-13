#ifndef HASH_MAP_H
#define HASH_MAP_H

// define the constants
#define MAX_WORD_SIZE 50
#define INITIAL_CAPACITY 16

// define the structurs:

// to keep track of how much a word was found after the key
typedef struct {
    char word[MAX_WORD_SIZE];
    int count;
} WordCount;

// a key and a list of struct WordCount
typedef struct {
    char key[MAX_WORD_SIZE];
    WordCount* followers;
    int followerCount;
    int followerCapacity;
} HashMapEntry;

// the hash map itself with a list of entries
typedef struct {
    HashMapEntry *entries;
    int size;
    int capacity;
} HashMap;

// public function used in other files (hopefully)
HashMapEntry* findEntry(HashMap* map, const char* key);
int isKeyInHashMap(HashMap *map, const char* key);
void prettyPrintHashMap(HashMap *map);
void addWordToHashMap(HashMap* map, char* key, char* follower);
void parseWord(HashMap* map, char* filename);


#endif
