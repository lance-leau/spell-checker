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

void correct_typos(GtkEntry* entry, char* new, char** words, int word_count);
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

int is_punctuation(char c) {
	char ponct[] = {'.', ',', '!', ':', ';', '?'};
	for (int i = 0; ponct[i] != '\0'; i ++) {
		if (c == ponct[i]) return 1;
	}
    return 0;
}

// Function to parse the input string into words
char** parser(const gchar* text, int* word_count) {
    int size = 10; // Initial capacity for the result array
    char** result = malloc(size * sizeof(char*)); // Allocate initial memory
    *word_count = 0; // Initialize word count

    const char* start = text;
    while (*text) {
        if (isspace(*text) || is_punctuation(*text)) {
            if (start != text) {
                int length = text - start;
                result[*word_count] = malloc((length + 1) * sizeof(char));
                strncpy(result[*word_count], start, length);
                result[*word_count][length] = '\0';
                (*word_count)++;
            }
            if (is_punctuation(*text)) {
                result[*word_count] = malloc(2 * sizeof(char));
                result[*word_count][0] = *text;
                result[*word_count][1] = '\0';
                (*word_count)++;
            }
            if (*word_count >= size) {
                size *= 2;
                result = realloc(result, size * sizeof(char*));
            }
            start = text + 1;
        }
        text++;
    }

    if (start != text) {
        int length = text - start;
        result[*word_count] = malloc((length + 1) * sizeof(char));
        strncpy(result[*word_count], start, length);
        result[*word_count][length] = '\0';
        (*word_count)++;
    }

    result[*word_count] = NULL; // Null-terminate the array of words

    return result;
}

// Function to free the allocated memory
void free_parsed_words(char** words) {
    for (int i = 0; words[i] != NULL; i++) {
        free(words[i]);
    }
    free(words);
}

static void on_entry_changed(GtkEntry *entry, gpointer user_data) {

	if(is_updating)
		return;


	is_updating = TRUE;

	const gchar *text = gtk_entry_get_text(entry);
	char *current_text = g_strdup(text); // Duplicate the current text for manipulation

	CallbackData *data = (CallbackData*)user_data;

	size_t n = strlen(text);
	if (n == 0) {
		is_updating = FALSE;
		g_free(current_text);
		return; // No text, just return
	}

	int isSpace = text[n - 1] == ' ' ? 1 : 0;
	int isPonct = text[n - 1] == '.' || text[n - 1] == ',' || text[n - 1] == ':' ? 1 : 0;

	if (isSpace) {
		int word_count;
    	char** words = parser(text, &word_count);

    	printf("Word count: %d\n", word_count);
    	for (int i = 0; words[i] != NULL; i++) {
        	printf("[%s]\n", words[i]);
    	}	

		char* cur = words[word_count-1];
		char* prev;
		if (word_count < 2) {
			prev = "_";
		} else {
			prev = words[word_count-2];
		}

		if (!isWord(data->tree, cur)) {
			if (isWord(data->treeNames, cur)) {
				printf("A name has been found");
				prev = "he";
			} else if (isWord(data->treePlace, cur)) {
				prev = "it";
			}
			is_updating = TRUE;
			char* tmp = fixWord(to_lower(cur), data->map, to_lower(prev), data->verbs, data->verb_count, data->treeNouns);
			correct_typos(entry, tmp, words, word_count);
			is_updating = FALSE;

			free(tmp);
		} else {
			is_updating = TRUE;
			char* tmp = fixWord(to_lower(cur), data->map, to_lower(prev), data->verbs, data->verb_count, data->treeNouns);
			correct_typos(entry, tmp, words, word_count);
			is_updating = FALSE;

			free(tmp);
			prev = cur;
		}

		free_parsed_words(words);
	}
	is_updating = FALSE;
	g_free(current_text);
}

void correct_typos(GtkEntry *entry, char *new_word, char** words, int word_count) {
    int total_length = 0;
    for (int i = 0; i < word_count - 1; i++) {
        total_length += strlen(words[i]) + 1; // Add 1 for the space or punctuation
    }
    total_length += strlen(new_word);

    char* new_text = malloc((total_length + 1) * sizeof(char));
    new_text[0] = '\0'; // Initialize the string

    for (int i = 0; i < word_count - 1; i++) {
        strcat(new_text, words[i]);
        strcat(new_text, " ");
    }

    strcat(new_text, new_word);
	strcat(new_text, " ");

    gtk_entry_set_text(entry, new_text);

    free(new_text);
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


