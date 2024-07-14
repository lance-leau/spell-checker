#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./Tree/tree.h"
#include "./word_prediction/hash_map.c"
#include "./levenshtein/levenshtein.h"

#define WORD_MAX_SIZE 45
#define MAX_WORDS 500

char** parseTextToWord(char* input) {
    char** ret = calloc(MAX_WORDS, sizeof(char*));
    int wordCount = 0;

    char* curr = calloc(WORD_MAX_SIZE, sizeof(char));
    int wordSize = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        char ch = input[i];
        if ('A' <= ch && ch <= 'Z') {
            ch += 'a' - 'A';  // Convert to lowercase
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
    if (wordSize > 0) { // Last word
        curr[wordSize] = '\0';
        ret[wordCount] = curr;
    }

    return ret;
}

int main(int argc, char** argv) {
    // Init tree ---------------------------------------------------
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
    filterLowFrequencyWords(map, 1);
    sortHashMap(map);

    // Parse sentence to words --------------------------------------
    char* txt = "helzo I wuold likr somme cheise.";

    char** wordArr = parseTextToWord(txt);
    char* prev = "_";

    for (int i = 0; wordArr[i] != NULL; i++) {
        if (!isWord(tree, wordArr[i])) {
            HashMapEntry* entry = findEntry(map, prev);
            int min = 100;
            char* currBest = calloc(WORD_MAX_SIZE, sizeof(char));
            for (int j = 0; j < entry->followerCount; j++) {
                int dist = distance(entry->followers[j], wordArr[i]);
                if (dist < min) {
                    min = dist;
                    strcpy(currBest, entry->followers[j]);
                }
            }
            // TODO call phonetic distance.
            wordArr[i] = currBest;
        }
        prev = wordArr[i];
    }

    for (int i = 0; wordArr[i] != NULL; i++) {
        printf("%s ", wordArr[i]);
    }
    printf("\n");

    // Free allocated memory
    for (int i = 0; i < MAX_WORDS && wordArr[i] != NULL; i++) {
        free(wordArr[i]);
    }
    free(wordArr);

    return 0;
}
