#include <gtk/gtk.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "../Tree/tree.h"
#include "../word_prediction/hash_map.h"
#include "../levenshtein/levenshtein.h"
#include "../metaphone/metaphone.h"

#define WORD_MAX_SIZE 45

typedef struct {
    struct node* tree;
    struct node* treeNames;
    struct node* treePlace;
    HashMap* map;
} CallbackData;

char* fixWord(char* word, HashMap* map, char* prev) {
    HashMapEntry* entry = findEntry(map, prev);
    if (entry != NULL) {
        int min = 100;
        char* currBest = NULL;
        for (int j = 0; j < entry->followerCount; j++) {
            int soundSame = isEqual(entry->followers[j].word, word);
            int dist = distance(entry->followers[j].word, word);
            if (soundSame == 1 && entry->followers[j].word[0] == word[0] && dist < 3) {
                if (dist < min) {
                    min = dist;
                    currBest = entry->followers[j].word;
                }
            }
        }
        if (currBest != NULL) {
            printf("Fixed <%s> to <%s>\n", word, currBest);
            return strdup(currBest);
        } else {
            printf("currBest is NULL\n");
            return strdup(word); // Fallback to original word
        }
    } else {
        printf("no entry found for %s\n", word);
        return strdup(word); // Fallback to original word
    }
}

char** parser(const gchar* text) {
    size_t n = strlen(text);
    char** r = malloc(sizeof(char*) * 2); // Allocate space for 2 words
    size_t indW = 0;
    size_t indR = 0;
    char* w = malloc(sizeof(char) * WORD_MAX_SIZE);
    int i = n - 1;

    while (i >= 0 && indR < 2) {
        if (text[i] == ' ' || i == 0) {
            if (indW > 0 || i == 0) {
                if (i == 0 && text[i] != ' ') 
                    w[indW++] = text[i]; 
                    
                w[indW] = '\0';
                // Reverse the word
                for (size_t j = 0; j < indW / 2; j++) {
                    char temp = w[j];
                    w[j] = w[indW - j - 1];
                    w[indW - j - 1] = temp;
                }
                r[indR] = malloc(sizeof(char) * (indW + 1));
                strcpy(r[indR], w);
                indR++;
                indW = 0;
            }
        } else {
            w[indW] = text[i];
            indW++;
        }
        i--;
    }
    
    free(w);
    return r;
}

// Callback function to handle text changes in the entry widget
static void on_entry_changed(GtkEntry *entry, gpointer user_data) {
    const gchar *text = gtk_entry_get_text(entry);
    CallbackData *data = (CallbackData *)user_data;

    size_t n = strlen(text);
    if (n == 0) 
        return;  // No text, just return

    int isSpace = text[n - 1] == ' ' ? 1 : 0;

    if (isSpace) {
        char** r = parser(text);
        char* cur = r[0];
        char* prev = (r[1] != NULL) ? r[1] : "_";

        if (!isWord(data->tree, cur)) {
            if (isWord(data->treeNames, cur)) // If it is a name, put prev as he/she
                prev = "he";
            else if (isWord(data->treePlace, cur)) // If it is a place (countries/towns), put prev as it
                prev = "it";
            else 
	    {
		//prev = fixWord(cur, data->map, prev);
		char* tmp = fixWord(cur, data->map, prev);
                printf("Word: %s\nCorrected Word: %s\n\n", cur, tmp);

            }
        } else 
	{
	    char* t = fixWord(cur, data->map, prev);		
            printf("Word: %s\nCorrected Word: %s\n\n", cur,  t);
	    prev = cur;
        }

        free(r[0]);
        free(r[1]);
        free(r);
    }
}

int main(int argc, char *argv[]) {
    // Initialization of trees + hashmap
    struct node* tree = buildTreeFromFile("../Tree/text_100k.txt");
    struct node* treeNames = buildTreeFromFile("../Tree/names.txt");
    struct node* treePlace = buildTreeFromFile("../Tree/places2.txt");
    // Init Hash map
    HashMap* map = initHashMap();
    parseWord(map, "../word_prediction/text3.txt");
    sortWordFrequency(map);
    sortHashMap(map);

    CallbackData data = {
        .tree = tree,
        .treeNames = treeNames,
        .treePlace = treePlace,
        .map = map
    };

    GtkWidget *window;
    GtkWidget *entry;

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create a new window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Entry Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    // Create a new entry widget
    entry = gtk_entry_new();

    // Connect the "changed" signal of the entry widget to the callback function
    g_signal_connect(entry, "changed", G_CALLBACK(on_entry_changed), &data);

    // Add the entry widget to the window
    gtk_container_add(GTK_CONTAINER(window), entry);

    // Connect the "destroy" signal of the window to gtk_main_quit to close the application
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Show all widgets within the window
    gtk_widget_show_all(window);

    // Enter the main event loop
    gtk_main();

    // Free resources
    // Note: Make sure to properly free your allocated memory and resources here
    destroyTree(tree);
    destroyTree(treeNames);
    destroyTree(treePlace);
//    destroyHashMap(map);

    return 0;
}

