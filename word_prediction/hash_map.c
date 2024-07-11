#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_map.h"

// Initialize a hash map
HashMap* initHashMap() {
    HashMap *map = malloc(sizeof(HashMap));
    map->size = 0;
    map->capacity = INITIAL_CAPACITY;
    map->entries = malloc(sizeof(HashMapEntry) * map->capacity);
    return map;
}

// hashes the word to easily find it later
// in case of collision (unexpected/unlikely results) the problem is probably hear
unsigned int hash(const char *str) {
    unsigned int hash = 5381; // try using another number if there is a problem with hashing
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

// finds an entry by key
HashMapEntry* findEntry(HashMap *map, const char *key) {
    unsigned int idx = hash(key) % map->capacity;
    while (map->entries[idx].key[0] != '\0') {
        if (strcmp(map->entries[idx].key, key) == 0) {
            return &map->entries[idx];
        }
        idx = (idx + 1) % map->capacity;
    }
    return NULL;
}

// checks if key is in hash map
int isKeyInHashMap(HashMap *map, const char *key) {
    return findEntry(map, key) != NULL;
}

// resizes hash map
void resizeHashMap(HashMap *map) {
    int oldCapacity = map->capacity;
    HashMapEntry *oldEntries = map->entries;

    map->capacity *= 2;
    map->entries = malloc(sizeof(HashMapEntry) * map->capacity);
    map->size = 0;

    for (int i = 0; i < oldCapacity; i++) {
        if (oldEntries[i].key[0] != '\0') {
            for (int j = 0; j < oldEntries[i].followerCount; j++) {
                // Reinsert the followers
                for (int k = 0; k < oldEntries[i].followers[j].count; k++) {
                    addWordToHashMap(map, oldEntries[i].key, oldEntries[i].followers[j].word);
                }
            }
        }
    }

    free(oldEntries);
}

// adds a word to the given hash map
void addWordToHashMap(HashMap *map, const char *key, const char *follower) {
    if (map->size >= map->capacity / 2) {
        resizeHashMap(map);
    }

    unsigned int idx = hash(key) % map->capacity;
    while (map->entries[idx].key[0] != '\0') {
        if (strcmp(map->entries[idx].key, key) == 0) {
            break;
        }
        idx = (idx + 1) % map->capacity;
    }

    if (map->entries[idx].key[0] == '\0') {
        strcpy(map->entries[idx].key, key);
        map->entries[idx].followers = malloc(sizeof(WordCount) * INITIAL_CAPACITY);
        map->entries[idx].followerCount = 0;
        map->entries[idx].followerCapacity = INITIAL_CAPACITY;
        map->size++;
    }

    for (int i = 0; i < map->entries[idx].followerCount; i++) {
        if (strcmp(map->entries[idx].followers[i].word, follower) == 0) {
            map->entries[idx].followers[i].count++;
            return;
        }
    }

    if (map->entries[idx].followerCount >= map->entries[idx].followerCapacity) {
        map->entries[idx].followerCapacity *= 2;
        map->entries[idx].followers = realloc(map->entries[idx].followers, sizeof(WordCount) * map->entries[idx].followerCapacity);
    }

    strcpy(map->entries[idx].followers[map->entries[idx].followerCount].word, follower);
    map->entries[idx].followers[map->entries[idx].followerCount].count = 1;
    map->entries[idx].followerCount++;
}

// Pretty print <3
void prettyPrintHashMap(HashMap *map) {
    for (int i = 0; i < map->capacity; i++) {
        if (map->entries[i].key[0] != '\0') {
            printf("%s:", map->entries[i].key);
            for (int j = 0; j < map->entries[i].followerCount; j++) {
                printf(" %s (%d)", map->entries[i].followers[j].word, map->entries[i].followers[j].count);
            }
            printf("\n");
        }
    }
}

// reads a file and adds every word to the hash map
void parseWord(HashMap* map, char* filename) {
	FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

	char* currWord = calloc(MAX_WORD_SIZE, sizeof(char));
	int currWordSize = 0;

	char* prev = NULL;

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if ('A' <= ch && ch <= 'Z') {
			ch += 'a' - 'A';
		}
		if ('a' <= ch && ch <= 'z') {
			currWord[currWordSize] = ch;
			currWordSize++;
		} else {
			if (currWordSize != 0) {
				if (prev != NULL) {
					addWordToHashMap(map, prev, currWord);
				}
				prev = calloc(MAX_WORD_SIZE, sizeof(char));
				for (int i = 0; currWord[i] != 0; i++) {
					prev[i] = currWord[i];
				}
				free(currWord);
				currWord = calloc(MAX_WORD_SIZE, sizeof(char));
				currWordSize = 0;
			} else {
				prev = NULL;
			}
		}
    }

    fclose(file);
}

int main() {
    HashMap *map = initHashMap();
	parseWord(map, "text.txt");
    prettyPrintHashMap(map);
    return 0;
}
