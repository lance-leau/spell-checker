#ifndef HASH_MAP_H
#define HASH_MAP_H

// define the constants
#define MAX_WORD_SIZE 15
#define INITIAL_CAPACITY 65536
#define MAX_LINE 100000

// define the structures:

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
HashMap* initHashMap();
HashMapEntry* findEntry(HashMap* map, char* key);
int isKeyInHashMap(HashMap *map, char* key);
void addWordToHashMap(HashMap* map, char* key, char* follower);
void parseWord(HashMap* map, char* filename);
void sortWordFrequency(HashMap* map);
void filterLowFrequencyWords(HashMap* map, int threshold);
void sortHashMap(HashMap* map);
void prettyPrintHashMap(HashMap *map);

#endif
