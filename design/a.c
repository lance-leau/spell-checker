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

void correct_typos(GtkEntry *entry,  char *new, const char *prev, char** array);
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
	}


	if (entry != NULL) {
		puts(word);
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
//Parse the last words of the text
int is_punctuation(char c)
{
	return ispunct(c) && c!= '\'';
}


char** parser(const gchar* text) {
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
}

size_t get_size(char** a)
{
	size_t n = 0;

	while(a[n] != NULL)
		n++;
	return n;
}

static void on_entry_changed(GtkEntry *entry, gpointer user_data) {

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
	int isPonct = text[n - 1] == '.' || text[n - 1] == ',' || text[n - 1] == ':' ? 1 : 0;

	if (isSpace) {
		char** r = parser(text);
		size_t size = get_size(r);
		char* cur = r[size-1];
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
				puts(tmp);
				correct_typos(entry, tmp, prev, r);
				is_updating = FALSE;

				free(tmp);
			}
		} else {
			/*if(isWord(data->treeNouns, cur))
			  prev = "it";*/
			is_updating = TRUE;
			char* t = fixWord(to_lower(cur), data->map, to_lower(prev), data->verbs, data->verb_count, data->treeNouns);
			puts(t);
			correct_typos(entry, t, prev, r);
			is_updating = FALSE;

			free(t);
			prev = cur;
		}

		free(r[0]);
		free(r[1]);
		free(r);
	}
	is_updating = FALSE;
	g_free(current_text);
}

void reverse_char_array(char** array) {
    size_t size = get_size(array);
    for (size_t i = 0; i < size / 2; i++) {
        char* temp = array[i];
        array[i] = array[size - i - 1];
        array[size - i - 1] = temp;
    }
}

void correct_typos(GtkEntry *entry, char *new, const char* prev, char** array) {
    	printf("fghjklm");
	size_t size = get_size(array);
	
	printf("HERE");

    if (size > 0) {
        free(array[size-1]); // Free the previous last element if allocated
        array[size-1] = strdup(new); // Assign the new corrected word

        reverse_char_array(array);

	printf("HERE 2");

        // Construct the new text from the array
        GString *new_text = g_string_new("");
        for (size_t i = 0; i < size; i++) {
            g_string_append(new_text, array[i]);
            if (i < size - 1) {
                g_string_append(new_text, " ");
            }
        }

        // Set the new text in the GTK entry
        gtk_entry_set_text(entry, new_text->str);

        g_string_free(new_text, TRUE);

        // Free memory allocated for array elements
        for (size_t i = 0; i < size; i++) {
            free(array[i]);
        }
        free(array);
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
	parseWord(map, "./../word_prediction/text.txt");
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

