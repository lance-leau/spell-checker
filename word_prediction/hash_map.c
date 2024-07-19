#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_map.h"
#include <time.h>

// Initialize a hash map
HashMap* initHashMap() {
    HashMap* map = malloc(sizeof(HashMap));
    map->size = 0;
    map->capacity = INITIAL_CAPACITY;
    map->entries = calloc(map->capacity, sizeof(HashMapEntry));
    return map;
}

// hashes the word to easily find it later
// in case of collision (unexpected/unlikely results) the problem is probably here
unsigned int hash(char* str) {
    unsigned int hash = 0; // try using another number if there is a problem with hashing
    int c;
    while ((c = *str++))
        hash = (hash*31 + c) % INITIAL_CAPACITY;
    return hash;
}

// finds an entry by key
HashMapEntry* findEntry(HashMap* map, char* key) {
    unsigned int index = hash(key) % map->capacity;
	/*
	unsigned int index = 0;
	for (; index < map->size; index ++) {
		if (strcmp(map->entries[index].key, key)) break;
	}
	*/
    while (map->entries[index].key[0] != '\0') {
        if (strcmp(map->entries[index].key, key) == 0) {
            return &map->entries[index];
        }
        index = (index+1) % map->capacity;
    }
	printf("no entry found for %s\n", key);
    return NULL;
}

// checks if key is in hash map
int isKeyInHashMap(HashMap* map, char* key) {
    return findEntry(map, key) != NULL;
}

// resizes hash map
void resizeHashMap(HashMap* map) {
    int oldCapacity = map->capacity;
    HashMapEntry* oldEntries = map->entries;

    map->capacity *= 2;
    map->entries = calloc(map->capacity, sizeof(HashMapEntry));
    map->size = 0;

    for (int i = 0; i < oldCapacity; i++) {
        if (oldEntries[i].key[0] != '\0') {
            for (int j = 0; j < oldEntries[i].followerCount; j++) {
                for (int k = 0; k < oldEntries[i].followers[j].count; k++) {
                    addWordToHashMap(map, oldEntries[i].key, oldEntries[i].followers[j].word);
                }
            }
        }
    }

    free(oldEntries);
}

// adds a word to the given hash map
void addWordToHashMap(HashMap* map, char* key, char* follower) {
    if (map->size >= map->capacity / 2) {
        resizeHashMap(map);
    }

    unsigned int index = hash(key) % map->capacity;
    while (map->entries[index].key[0] != '\0') {
        if (strcmp(map->entries[index].key, key) == 0) {
            break;
        }
        index = (index + 1) % map->capacity;
    }

    if (map->entries[index].key[0] == '\0') {
        strcpy(map->entries[index].key, key);
        map->entries[index].followers = calloc(INITIAL_CAPACITY, sizeof(WordCount));
        map->entries[index].followerCount = 0;
        map->entries[index].followerCapacity = INITIAL_CAPACITY;
        map->size++;
    }

    for (int i = 0; i < map->entries[index].followerCount; i++) {
        if (strcmp(map->entries[index].followers[i].word, follower) == 0) {
            map->entries[index].followers[i].count++;
            return;
        }
    }

    if (map->entries[index].followerCount >= map->entries[index].followerCapacity) {
        map->entries[index].followerCapacity *= 2;
        map->entries[index].followers = realloc(map->entries[index].followers, sizeof(WordCount) * map->entries[index].followerCapacity);
	}
    

    strcpy(map->entries[index].followers[map->entries[index].followerCount].word, follower);
    map->entries[index].followers[map->entries[index].followerCount].count = 1;
    map->entries[index].followerCount++;
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
	FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

	char* currWord = calloc(MAX_WORD_SIZE, sizeof(char));
	int currWordSize = 0;

	char* prev = NULL;
	
	unsigned int currLine = 0;

    char ch;
    while (currLine <= MAX_LINE && (ch = fgetc(file)) != EOF) {
		if (ch == 44) continue;
		if (ch == '\n') currLine++;
        if ('A' <= ch && ch <= 'Z') {
			ch += 'a' - 'A';
		}
		if (('a' <= ch && ch <= 'z') || (ch == '\'' || ch == '-')) {
			currWord[currWordSize] = ch;
			currWordSize++;
		} else {
			if (currWordSize != 0) {
				if (prev != NULL) {
					addWordToHashMap(map, prev, currWord);
				} else {
					addWordToHashMap(map, "_", currWord);
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

int __compareHashMapEntry(const void* a, const void* b) {
    HashMapEntry* entryA = (HashMapEntry*)a;
    HashMapEntry* entryB = (HashMapEntry*)b;
    return strcmp(entryA->key, entryB->key);
}

// removes empty entries and sorts entries by key value alphabetically
void sortHashMap(HashMap* map) {
    int newSize = 0;
    for (int i = 0; i < map->capacity; i++) {
        if (map->entries[i].key[0] != '\0') {
            map->entries[newSize] = map->entries[i];
			map->entries[i].key[0] = '\0';
        	newSize++;
		}
    }
    map->size = newSize;
    qsort(map->entries, map->size, sizeof(HashMapEntry), __compareHashMapEntry);
}


int __compareWordCount(const void *a, const void *b) {
	WordCount* wc1 = (WordCount *)a;
	WordCount* wc2 = (WordCount *)b;
	return wc2->count - wc1->count;
}

void sortWordFrequency(HashMap* map) {
	for (int i = 0; i < map->capacity; i++) {
		if (map->entries[i].key[0] != '\0') {
			// of course there is a qsort already in implemented in <stdlib.h>, I knew that, I did not try to code it by hand
			qsort(map->entries[i].followers, map->entries[i].followerCount, sizeof(WordCount), __compareWordCount);
        }
    }
}

void filterLowFrequencyWords(HashMap* map, int threshhold) {
	// filterLowFreqencyWords() only works on sorted keys (first call sortWordFrequency)
	for (int i = 0; i < map->capacity; i++) {
		HashMapEntry entry = map->entries[i];
		if (entry.key[0] != '\0') {
			for (int j = 0; j < entry.followerCount; j++) {
				if (entry.followers[j].count <= threshhold) {
					map->entries[i].followerCount = j;
					if (j == 0) {
						map->entries[i].key[0] = '\0';
					}
					break;
				}
			}
		}
	}	
}


int main() {

	clock_t begin = clock();

    HashMap* map = initHashMap();
	parseWord(map, "text4.txt");
	sortWordFrequency(map);
    filterLowFrequencyWords(map, 1);
	sortHashMap(map);
	prettyPrintHashMap(map);

	/*
	HashMapEntry* entry = findEntry(map, "i");

	for (int i = 0; i < entry->followerCount; i++) {
		if (entry->followers[i].word[0] != 0) {
			printf("%s \n", entry->followers[i].word);
		} else {
			printf("NULL\n");
		}
	}
	*/
	printf("%i\n", map->capacity);

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("time: %lf\n", time_spent);

    return 0;
}

