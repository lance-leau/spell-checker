#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./Tree/tree.h"
#include "./word_prediction/hash_map.h"
#include "./levenshtein/levenshtein.h"

#define WORD_MAX_SIZE 45

char** parseTextToWord(char* input) {
	
	char** ret = calloc(1000, sizeof(char*));
	int wordCount = 0;

	char* curr = calloc(WORD_MAX_SIZE, sizeof(char));
	int wordSize = 0;

	for (int i = 0; input[i] != '\0'; i++) {
		char ch = input[i];
		if ('A' <= ch && ch <= 'Z') {
			ch -= 'a' - 'A';
		}
		if ('a' <= ch && ch <= 'z') {
			curr[wordSize] = ch;
			wordSize++;
		} else {
			if (wordSize > 0) {
				curr[wordSize] = '\0';
				ret[wordCount] = curr;
				wordCount++;
				curr = calloc(WORD_MAX_SIZE, sizeof(char));
				wordSize = 0;
			}
		}
	}

	return ret;
}

int main (int argc, char** argv) {
	
	// Init tree ---------------------------------------------------
	struct node* tree = initTree();
    
    FILE* file;
    char line[256];
    file = fopen("./Tree/text_100k.txt", "r");
 
    if (NULL == file) {
        printf("file can't be opened \n");
    }
 
    // printf("content of this file are \n");
 
    while (fgets(line, sizeof(line), file) != NULL) 
    {
        line[strcspn(line, "\n\r")] = '\0';
        addWord(tree,line);
    }
    fclose(file);

	// Init Hash map ------------------------------------------------
    HashMap* map = initHashMap();
	parseWord(map, "./word_prediction/text3.txt");
	sortWordFrequency(map);
    filterLowFrequencyWords(map, 1);
	sortHashMap(map);


	// Parse sentence to words --------------------------------------
	char* txt = "helzo I wuold likr somme cheise.";
	
	
	char** ret = calloc(1000, sizeof(char*));
	// char ret[500][500];

	char** wordArr = parseTextToWord(txt);
	char* prev = "_";

	for (int i = 0; wordArr[i][0] != 0; i++) {
		if (!isWord(tree, wordArr[i])) {
			HashMapEntry* entry = findEntry(map, prev);
			int min = 100;
			char* currBest = calloc(MAX_WORD_SIZE, sizeof(char));
			for (int j = 0; j < entry->followerCount; j++) {
				int dist = distance(entry->followers[j].word, wordArr[i]);
				if (dist < min) {
					min = dist;
					strcpy(currBest, entry->followers[j].word);
				}
			}
			// TODO call phonetic distance.
			ret[i] = currBest;
		} else {
			ret[i] = wordArr[i];
		}
	}

	for (int i = 0; ret[i] != 0; i++) {
		printf("%s ", ret[i]);
	}
	printf("\n");
	return 0;
}
