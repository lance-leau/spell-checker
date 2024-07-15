#include "tree.h"



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

json_object* serializeNode(struct node* root) {
    json_object* jsonObj = json_object_new_object();

    // Add 'value' field
    json_object_object_add(jsonObj, "value", json_object_new_string_len(&root->value, 1));

    // Add 'isWord' field
    json_object_object_add(jsonObj, "isWord", json_object_new_int(root->isWord));

    // Add 'children' field as an array
    json_object* childrenArray = json_object_new_array();
    for (int i = 0; i < 26; ++i) {
        if (root->children[i] != NULL) {
            json_object_array_add(childrenArray, serializeNode(root->children[i]));
        }
    }
    json_object_object_add(jsonObj, "children", childrenArray);

    return jsonObj;
}

// Function to write JSON data to a file
void writeJSONToFile(json_object* jsonObj, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    const char* jsonString = json_object_to_json_string_ext(jsonObj, JSON_C_TO_STRING_PRETTY);
    fprintf(file, "%s\n", jsonString);

    fclose(file);
}



