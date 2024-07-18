#include "tree.h"



struct node* initTree() {
        return createNode('_');
}

struct node* createNode (char value) {
        struct node* newNode = (struct node*)malloc(sizeof(struct node));

        newNode->value = value;
        newNode->children = (struct node**)calloc(26, sizeof(struct node*));
        newNode->isWord = 0;

        return newNode;
}

void addWord(struct node* tree, const char* s)
{
        struct node* current = tree;

        for(size_t i = 0; i < strlen(s); i++)
        {
                int index = tolower(s[i]) - 'a';

                if(current->children[index] == NULL)
                {
                        current->children[index] = createNode(tolower(s[i]));
                }
                current = current->children[index];
        }
        current->isWord = 1;
}

int isWord(struct node* tree, const char* s)
{
        struct node* current = tree;

        for(size_t i = 0; i < strlen(s); i++)
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

struct node*  buildTreeFromFile(char* text)
{
	struct node* tree = initTree();

    
   	FILE* file;
   	char line[256];
     	file = fopen(text, "r");
 
    	if (NULL == file) {
        	printf("file can't be opened \n");
   	 }
 
 
   	 while (fgets(line, sizeof(line), file) != NULL	) 
    	{
        	line[strcspn(line, "\n\r")] = '\0';
        	addWord(tree,line);
    	}
    	fclose(file);

	return tree;

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

void destroyTree(struct node* root) {
    if (root == NULL) {
        return;
    }
    
    if (root->children != NULL) {
        int i = 0;
        while (root->children[i] != NULL) {
            destroyTree(root->children[i]);
            i++;
        }
        free(root->children);
    }
    
    free(root);
}
/*
int main (int argc, char** argv) {

	struct node* words = buildTreeFromFile("text_100k.txt");
	prettyPrint(words,0);
	free(words);
   

        return 0;
}*/
