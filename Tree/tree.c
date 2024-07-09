#include <stdio.h>
#include <stdlib.h>

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

struct node* initTree() {
	return createNode('_');
}


void prettyPrint(struct node* tree, int level) {
    if (tree == NULL) return;

    for (int i = 0; i < level; i++) {
        printf("-");
    }

    printf("%c", tree->value);
    if (tree->isWord) {
        printf(" (word)");
    }
    printf("\n");

   for (int i = 0; i < 26; i++) {
        prettyPrint(tree->children[i], level + 1);
    }
}


int main (int argc, char** argv) {
	struct node* tree = initTree();
	
	prettyPrint(tree, 0);
	return 0;
}
