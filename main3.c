#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "./Tree/tree.h"
#include "./word_prediction/hash_map.h"
#include "./levenshtein/levenshtein.h"
#include "./metaphone/metaphone.h"

#define WORD_MAX_SIZE 45

char** parseTextToWord(char* input) {
	
	char** ret = calloc(1000, sizeof(char*));
	int wordCount = 0;

	char* curr = calloc(WORD_MAX_SIZE, sizeof(char));
	int wordSize = 0;

	for (int i = 0; input[i] != '\0'; i++) {
		char ch = input[i];
		if ('A' <= ch && ch <= 'Z') {
			ch = tolower(ch);
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

	if (wordSize > 0) {
        curr[wordSize] = '\0';
        ret[wordCount] = curr;
        wordCount++;
    } else {
        free(curr);
    }

	return ret;
}

int main (int argc, char** argv) {
	struct node* tree = initTree();
    
    FILE* file;
    char line[256];
    file = fopen("./Tree/text_100k.txt", "r");
 
    if (NULL == file) {
        printf("file can't be opened \n");
        return 1;
    }
 
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n\r")] = '\0';
        addWord(tree, line);
    }
    fclose(file);

    // Init Hash map ------------------------------------------------
    HashMap* map = initHashMap();
    parseWord(map, "./word_prediction/text3.txt");
    sortWordFrequency(map);
   // filterLowFrequencyWords(map, 1);
    sortHashMap(map);

    // Parse sentence to words --------------------------------------
    char* txt = "So he hurreid on keeoing to the rough raod that ran down the valley";
    char** wordArr = parseTextToWord(txt);
    char* prev = "_";

    char** ret = calloc(1000, sizeof(char*));

    for (int i = 0; wordArr[i] != NULL; i++) {
		printf("yo1\n");
        if (!isWord(tree, wordArr[i])) {
            HashMapEntry* entry = findEntry(map, prev);
            if (entry != NULL) {
                int min = 100;
                char* currBest = NULL;
                for (int j = 0; j < entry->followerCount; j++) {
		    int soundSame = isEqual(entry->followers[j].word, wordArr[i]);
		    int dist = distance(entry->followers[j].word, wordArr[i]);
		    if(soundSame == 1 && entry->followers[j].word[0] == wordArr[i][0] && dist < 3)
		    {
                   	 if (dist < min)
			 {
                        	min = dist;
                       		 currBest = entry->followers[j].word;
			 }
		    }
				 
                }
                if (currBest != NULL) {
		    		printf("Current Best: %s\n", currBest);
                    ret[i] = strdup(currBest);
                } else {
					printf("currBest us NULL\n");
                    ret[i] = strdup(wordArr[i]); // Fallback to original word
                }
            } else {
				printf("no entry found for %s\n", wordArr[i]);
                ret[i] = strdup(wordArr[i]); // Fallback to original word
            }
        } else {
            ret[i] = strdup(wordArr[i]);
        }
        prev = wordArr[i];
    }

    for (int i = 0; ret[i] != NULL; i++) {
        printf("%s ", ret[i]);
        free(ret[i]);
    }
    printf("\n");

    free(ret);

    // Free the word array
    for (int i = 0; wordArr[i] != NULL; i++) {
        free(wordArr[i]);
    }
    free(wordArr);	
/*	char** p = parseTextToWord("Hello this is a test");
	
	for (int i = 0; i < 5; i++) 
	{
   		 printf("%s\n", p[i]);
  	}
*/

	return 0;
}
