#include "tree.c"

int main (int argc, char** argv) {
    struct node* tree = initTree();


    FILE* file;
    char line[256];
    file = fopen("list_words.txt", "r");

    if (NULL == file) {
        printf("file can't be opened \n");
    }

    printf("content of this file are \n");

    while (fgets(line, sizeof(line), file) != NULL)
    {
        line[strcspn(line, "\n\r")] = '\0';
        addWord(tree,line);
    }
    fclose(file);

    int k = isWord(tree, "aaa");

    if(k == 1)
        printf("Hello is in tree \n");
    else
        printf("Hello is not in tree \n");

    json_object* jsonRoot = serializeNode(tree);

    // Write JSON data to a file
    writeJSONToFile(jsonRoot, "tree.json");

    // Clean up
    json_object_put(jsonRoot);


        return 0;
}

