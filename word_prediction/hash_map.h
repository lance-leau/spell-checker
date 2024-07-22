#ifndef HASH_MAP_H
#define HASH_MAP_H

// Define the constants --------------------------------------------------------
#define MAX_WORD_SIZE 15
#define INITIAL_CAPACITY 200000
#define MAX_LINE 1000000

// Define the structures ------------------------------------------------------
typedef struct {
// to keep track of how much a word was found after the key
    char word[MAX_WORD_SIZE];
    int count;
} WordCount;


typedef struct {
// a key and a list of struct WordCount
    char key[MAX_WORD_SIZE];
    WordCount* followers;
    int followerCount;
    int followerCapacity;
} HashMapEntry;


typedef struct {
// the hash map itself with a list of entries
    HashMapEntry *entries;
    int size;
    int capacity;
} HashMap;


typedef struct {
// this is all the data a parser thread needs to parse
	HashMap* map;
	char* filename;
	int startLine;
	int endLine;
} ThreadData;

// Declare the functions -------------------------------------------------------
HashMap* initHashMap();
HashMapEntry* findEntry(HashMap* map, char* key);
int isKeyInHashMap(HashMap *map, char* key);
void addWordToHashMap(HashMap* map, char* key, char* follower);
void sortWordFrequency(HashMap* map);
void filterLowFrequencyWords(HashMap* map, int threshold);
void prettyPrintHashMap(HashMap *map);

void runThreads(int numThreads, HashMap* map, char* filename);

#endif
