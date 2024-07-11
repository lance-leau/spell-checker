#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void addWord(struct node* tree, const char* s)
{
	struct node* current = tree;

	for(size_t i = 0, i < strlen(s); i++)
	{
		int index = tolower(s[i]) - 'a';

		if(current->children[index] == NULL)
		{
			current->children[index] = createNode(tolower(s[i]);
		}
		current = current->children[index];
	}
}

int isWord(struct node* tree, const char* s)
{
        struct node* current = tree;

        for(size_t i = 0, i < strlen(s); i++)
        {
                int index = tolower(s[i]) - 'a';

                if(current->children[index] == NULL)
                {
                        return 0;
                }
                current = current->children[index];
        }
	return current->isWord;
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
