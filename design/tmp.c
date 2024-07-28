#include <gtk/gtk.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "../Tree/tree.h"
#include "../word_prediction/hash_map.h"
#include "../levenshtein/levenshtein.h"
#include "../metaphone/metaphone.h"
#include "verb.h"
#define WORD_MAX_SIZE 45

void correct_typos(GtkEntry *entry, const char *old, char *new, const char *prev);
static gboolean is_updating = FALSE;
typedef struct {
	struct node* tree;
	struct node* treeNames;
	struct node* treePlace;
	struct node* treeNouns;
	HashMap* map;
	Verb* verbs;
	size_t verb_count;
} CallbackData;

char* to_lower(char* c)
{
	size_t n = strlen(c);
	char* r = malloc(sizeof(char) * (n+1));

	for(size_t i = 0; i < n; i++)
	{
		r[i] = tolower(c[i]);
	}
	r[n] = 0;

	return r;
}


char* fixWord(char* word, HashMap* map, char* prev, Verb* verbs, size_t verb_count, struct node* treeNouns)
{
	HashMapEntry* entry = findEntry(map, prev);
	printf("THE PREV WORD IS %s \n", prev);
	if(strcmp(prev,"_") != 0 && isWord(treeNouns,prev))
	{
		isWord(treeNouns,prev) ? printf("YES") : printf("NO");
		prev = "it";
		entry = findEntry(map, "it");
	}

	if (entry != NULL) {
		puts(word);
		int min = 100;
		char* currBest = NULL;
		for (int j = 0; j < entry->followerCount; j++) {
			if (strcmp(entry->followers[j].word, "") != 0) {
				int soundSame = isEqual(entry->followers[j].word, word);
				int dist = distance(entry->followers[j].word, word);
				if (soundSame == 1 && entry->followers[j].word[0] == word[0] && dist < 3) {
					if (dist < min) {
						min = dist;
						currBest = entry->followers[j].word;
					}
				}
			}
			/*if(strcmp(prev,"_") == 1 && isWord(treeNouns, prev))
			  {
			  isWord(treeNouns,prev) ? printf("YES") : printf("NO");
			  prev = "it";
			  }*/

		}
		if (currBest != NULL) {

			printf("Fixed <%s> to <%s>\n", word, currBest);
			//isWord(treeNouns,prev) ? printf("YES\n") : printf("NO\n");
			/*if(strcmp(prev,"_") == 1 && isWord(treeNouns, prev))
			  {
			  isWord(treeNouns,prev) ? printf("YES") : printf("NO");
			  prev = "it";
			  }*/

			if(is_pronoun(prev))
			{	
				currBest = correct_verb_form(prev, currBest,verbs,verb_count);
				if (currBest) {
					printf("Corrected verb: %s\n", currBest);
					//free(corrected_verb);
				}

			}
			return strdup(currBest);
		} else {

			word = correct_verb_form(prev, word,verbs,verb_count);
			if (word) {
				printf("Corrected verb: %s\n", word);
				//free(corrected_verb);
			}
			printf("currBest is NULL\n");
			return strdup(word); // Fallback to original word
		}
	} else {
		word = correct_verb_form(prev, word,verbs,verb_count);
		if (word) {
			printf("Corrected verb: %s\n", word);
			//free(corrected_verb);
		}

		printf("no entry found for %s\n", word);
		return strdup(word); // Fallback to original word

	}
}

int is_punctuation(char c)
{
	return ispunct(c) && c!= '\'';
}


/*char** parser(const gchar* text) {
    size_t n = strlen(text);
    char** r = malloc(sizeof(char*) * (n + 1)); // Allocate space for n+1 words (worst case, each character is a separate word)
    size_t indW = 0;
    size_t indR = 0;
    char* w = malloc(sizeof(char) * WORD_MAX_SIZE);
    int i = n - 1;

    while (i >= 0) {
        if (text[i] == ' ' || is_punctuation(text[i]) || i == 0) {
            if (indW > 0 || i == 0) {
                if (i == 0 && text[i] != ' ' && !is_punctuation(text[i])) {
                    w[indW++] = text[i];
                }
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
            if (is_punctuation(text[i])) {
                r[indR] = malloc(sizeof(char) * 2);
                r[indR][0] = text[i];
                r[indR][1] = '\0';
                indR++;
            }
        } else {
            w[indW++] = text[i];
        }
        i--;
    }

    free(w);
    r[indR] = NULL; 
    return r;
}*/

char** parser(const gchar* text) {
    size_t n = strlen(text);
    char** r = malloc(sizeof(char*) * (n + 1)); // Allocate space for n+1 words (worst case, each character is a separate word)
    size_t indW = 0;
    size_t indR = 0;
    char* w = malloc(sizeof(char) * (WORD_MAX_SIZE + 1));
    int i = n - 1;

    while (i >= 0) {
        if (text[i] == ' ' || is_punctuation(text[i]) || i == 0) {
            if (indW > 0 || i == 0) {
                if (i == 0 && text[i] != ' ' && !is_punctuation(text[i])) {
                    w[indW++] = text[i];
                }
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
            if (is_punctuation(text[i])) {
                r[indR] = malloc(sizeof(char) * 2);
                r[indR][0] = text[i];
                r[indR][1] = '\0';
                indR++;
            }
        } else {
            w[indW++] = text[i];
        }
        i--;
    }

    free(w);
    r[indR] = NULL;
    return r;
}


size_t get_length(char** c)
{
	size_t n = 0;

	while(c[n] != NULL)
		n++;
	return n;
}

/*static void on_entry_changed(GtkEntry *entry, gpointer user_data) {

	if(is_updating)
		return;


	is_updating = TRUE;

	const gchar *text = gtk_entry_get_text(entry);
	char *current_text = g_strdup(text); // Duplicate the current text for manipulation

	CallbackData *data = (CallbackData *)user_data;

	size_t n = strlen(text);
	if (n == 0) {
		is_updating = FALSE;
		g_free(current_text);
		return; // No text, just return
	}

	int isSpace = text[n - 1] == ' ' ? 1 : 0;

	if (isSpace) {
		char** r = parser(text);
		size_t size = get_length(r);
		char* cur = (size > 0) ? r[size-1] : "_";;
		char* prev = (size > 1) ? r[size-2] : "_";
		//cur = to_lower(cur);
		//prev = to_lower(prev);

		if (!isWord(data->tree, cur)) {
			if (isWord(data->treeNames, cur)) {
				printf("A name has been found");
				prev = "he";
			} else if (isWord(data->treePlace, cur)) {
				prev = "it";
			} else {
				is_updating = TRUE;
				char* tmp = fixWord(to_lower(cur), data->map, to_lower(prev), data->verbs, data->verb_count, data->treeNouns);
				correct_typos(entry, cur, tmp, prev);
				is_updating = FALSE;

				free(tmp);
			}
		} else {
			is_updating = TRUE;
			char* tmp = fixWord(to_lower(cur), data->map, to_lower(prev), data->verbs, data->verb_count, data->treeNouns);
			correct_typos(entry, cur, tmp, prev);
			is_updating = FALSE;

			free(tmp);
			prev = cur;
		}

		free(r[0]);
		free(r[1]);
		free(r);
	}
	is_updating = FALSE;
	g_free(current_text);
}
*/

static void on_entry_changed(GtkEntry *entry, gpointer user_data) {
    if (is_updating)
        return;

    is_updating = TRUE;

    const gchar *text = gtk_entry_get_text(entry);
    char *current_text = g_strdup(text); // Duplicate the current text for manipulation

    CallbackData *data = (CallbackData *)user_data;

    size_t n = strlen(text);
    if (n == 0) {
        is_updating = FALSE;
        g_free(current_text);
        return; // No text, just return
    }

    int isSpace = text[n - 1] == ' ' ? 1 : 0;

    if (isSpace) {
        char** r = parser(text);
        size_t size = get_length(r);
        char* cur = (size > 0) ? r[size-1] : "_";
        char* prev = (size > 1) ? r[size-2] : "_";

        if (!isWord(data->tree, cur)) {
            if (isWord(data->treeNames, cur)) {
                printf("A name has been found");
                prev = "he";
            } else if (isWord(data->treePlace, cur)) {
                prev = "it";
            } else {
                is_updating = TRUE;
                char* tmp = fixWord(to_lower(cur), data->map, to_lower(prev), data->verbs, data->verb_count, data->treeNouns);
                correct_typos(entry, cur, tmp, prev);
                is_updating = FALSE;

                free(tmp);
            }
        } else {
            is_updating = TRUE;
            char* tmp = fixWord(to_lower(cur), data->map, to_lower(prev), data->verbs, data->verb_count, data->treeNouns);
            correct_typos(entry, cur, tmp, prev);
            is_updating = FALSE;

            free(tmp);
        }

        for (size_t i = 0; i < size; i++) {
            free(r[i]);
        }
        free(r);
    }
    is_updating = FALSE;
    g_free(current_text);
}

void correct_typos(GtkEntry *entry, const char *old, char *new, const char* prev) {
    const gchar *text = gtk_entry_get_text(entry);
    char *pos = strstr(text, old);

    if (pos != NULL) {
        printf("PREV IS: %s\n", prev);
        if(strcmp(prev, "_") == 0) {
            new[0] = toupper(new[0]);
            printf("TOUPPER: %s\n", new);
        }

        // Save the current cursor position
        gint cursor_position = gtk_editable_get_position(GTK_EDITABLE(entry));
        gint offset = pos - text; // Offset where the old word starts

        // Create new text with the correction
        size_t old_len = strlen(old);
        size_t new_len = strlen(new);
        gchar *new_text = g_malloc(strlen(text) - old_len + new_len + 1);

        // Copy text before the old word
        strncpy(new_text, text, offset);
        new_text[offset] = '\0';

        // Append new word and the rest of the original text
        strcat(new_text, new);
        strcat(new_text, pos + old_len);

        // Block signal handler to prevent recursion
        g_signal_handlers_block_by_func(entry, G_CALLBACK(on_entry_changed), NULL);
        gtk_entry_set_text(entry, new_text);

        // Calculate the new cursor position
        gint new_cursor_position = offset + new_len;
        gtk_editable_set_position(GTK_EDITABLE(entry), new_cursor_position);

        g_signal_handlers_unblock_by_func(entry, G_CALLBACK(on_entry_changed), NULL);

        g_free(new_text);
    }
}


int main(int argc, char *argv[]) {
	
	// Initialization of trees + hashmap
	struct node* tree = buildTreeFromFile("../Tree/text_100k.txt");
	struct node* treeNames = buildTreeFromFile("../Tree/names.txt");
	struct node* treePlace = buildTreeFromFile("../Tree/places2.txt");
	struct node* treeNouns = buildTreeFromFile("../Tree/text_nouns.txt");
	
	// Init Hash map
	HashMap* map = initHashMap();
	int numThreads = 16;
	runThreads(numThreads, map, "./../word_prediction/text.txt");
	sortWordFrequency(map);

	//Init the verb 
	const char* filename = "verbs_parsed.txt";
	Verb* verbs;
	size_t verb_count = load_verbs(filename, &verbs);
	if (verb_count == 0) {
		fprintf(stderr, "Failed to load verbs\n");
		return 1;
	}


	CallbackData data = {
		.tree = tree,
		.treeNames = treeNames,
		.treePlace = treePlace,
		.treeNouns = treeNouns,
		.map = map,
		.verbs = verbs,
		.verb_count = verb_count
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
	destroyTree(treeNouns);
	//    destroyHashMap(map);

	return 0;
}


