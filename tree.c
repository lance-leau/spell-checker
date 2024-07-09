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

int addWord(struct node* tree, char* word) {
	struct node* curr = tree;

	for (int i = 0; word[i] != '\0'; i++) {

		char l = word[i];
		int index = l - 'a';

		if (curr->children[index] == NULL) {
			curr ->children[index] = createNode(l);
		}

		curr = curr->children[index];
	}

	if (curr->isWord) {
		printf("%s is already in the tree\n", word);
		return 0;
	}

	curr->isWord = 1;
	printf("added %s to the tree\n", word);
	return 1;
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
	addWord(tree, "bear");
	addWord(tree, "bell");
	addWord(tree, "bid");
	addWord(tree, "bull");
	addWord(tree, "buy");
	addWord(tree, "sell");
	addWord(tree, "stock");
	addWord(tree, "stop");

	prettyPrint(tree, 0);
	return 0;
}
