#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <json-c/json.h>

struct node {
    char value;
    struct node** children;
    int isWord;
};

struct node* createNode(char value);
struct node* initTree();
void addWord(struct node* tree, const char* s);
int isWord(struct node* tree, const char* s);
void prettyPrint(struct node* tree, int level);
json_object* serializeNode(struct node* root);
void writeJSONToFile(json_object* jsonObj, const char* filename);
struct node* deserializeNode(json_object* jsonObj);
struct node* readJSONFromFile(const char* filename);
#endif // TRIE_H

