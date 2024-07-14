#pragma once
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

struct node* createNode (char value) {
        struct node* newNode = (struct node*)malloc(sizeof(struct node));

        newNode->value = value;
        newNode->children = (struct node**)calloc(26, sizeof(struct node*));
        newNode->isWord = 0;

        return newNode;
}

struct node* initTree();
void addWord(struct node* tree, const char* s);
int isWord(struct node* tree, const char* s);
void prettyPrint(struct node* tree, int level);
json_object* serializeNode(struct node* root);
void writeJSONToFile(json_object* jsonObj, const char* filename);
void prettyPrint(struct node* tree, int level);
json_object* serializeNode(struct node* root);
void writeJSONToFile(json_object* jsonObj, const char* filename);

