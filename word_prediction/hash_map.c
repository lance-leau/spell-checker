#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_SIZE 20
#define INITIAL_CAPACITY 16

// Structure to store word counts
typedef struct {
    char word[MAX_WORD_SIZE];
    int count;
} WordCount;

// Structure to store the hash map entry
typedef struct {
    char key[MAX_WORD_SIZE];
    WordCount *followers;
    int followerCount;
    int followerCapacity;
} HashMapEntry;

// Structure to store the hash map
typedef struct {
    HashMapEntry *entries;
    int size;
    int capacity;
} HashMap;

void addWordToHashMap(HashMap *map, const char *key, const char *follower);


// Initialize a hash map
HashMap* initHashMap() {
    HashMap *map = malloc(sizeof(HashMap));
    map->size = 0;
    map->capacity = INITIAL_CAPACITY;
    map->entries = malloc(sizeof(HashMapEntry) * map->capacity);
    return map;
}

// Hash function for strings
unsigned int hash(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

// Find an entry by key
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

// Check if key is in hash map
int isKeyInHashMap(HashMap *map, const char *key) {
    return findEntry(map, key) != NULL;
}

// Resize hash map
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

// Add a word to the hash map
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

// Pretty print the hash map
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

// Free the hash map
void freeHashMap(HashMap *map) {
    for (int i = 0; i < map->capacity; i++) {
        if (map->entries[i].key[0] != '\0') {
            free(map->entries[i].followers);
        }
    }
    free(map->entries);
    free(map);
}

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
    /*
	addWordToHashMap(map, "hello", "world");
    addWordToHashMap(map, "hello", "there");
    addWordToHashMap(map, "hello", "world");
    addWordToHashMap(map, "hi", "there");
	*/
	parseWord(map, "text.txt");
    prettyPrintHashMap(map);
    freeHashMap(map);
    return 0;
}
