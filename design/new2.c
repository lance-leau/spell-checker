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

typedef struct {
	struct node* tree;
	struct node* treeNames;
	struct node* treePlace;
	HashMap* map;
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


char* fixWord(char* word, HashMap* map, char* prev) {
	HashMapEntry* entry = findEntry(map, prev);
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
		}
		if (currBest != NULL) {
			printf("Fixed <%s> to <%s>\n", word, currBest);
			if(is_pronoun(prev))
			{
				const char* filename = "verbs_parsed.txt";
				Verb* verbs;
				size_t verb_count = load_verbs(filename, &verbs);
				if (verb_count == 0) {
					fprintf(stderr, "Failed to load verbs\n");
					return 1;
				}

				currBest = correct_verb_form(prev, currBest,verbs,verb_count);
				if (currBest) {
					printf("Corrected verb: %s\n", currBest);
					//free(corrected_verb);
				}

			}
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
//Parse the last words of the text
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

	if (indR == 2 && r[1][strlen(r[1]) - 1] == '.') {
		free(r[1]);
		r[1] = malloc(sizeof(char) * 2);
		strcpy(r[1], "_");
	}

	free(w);
	return r;
}



static void on_entry_changed(GtkEntry *entry, gpointer user_data) {
	const gchar *text = gtk_entry_get_text(entry);
	char *current_text = g_strdup(text); // Duplicate the current text for manipulation

	CallbackData *data = (CallbackData *)user_data;

	size_t n = strlen(text);
	if (n == 0) {
		g_free(current_text);
		return; // No text, just return
	}

	int isSpace = text[n - 1] == ' ' ? 1 : 0;
	int isPonct = text[n - 1] == '.' || text[n - 1] == ',' || text[n - 1] == ':' ? 1 : 0;

	if (isSpace) {
		char** r = parser(text);
		char* cur = r[0];
		char* prev = (r[1] != NULL) ? r[1] : "_";
		//cur = to_lower(cur);
		//prev = to_lower(prev);

		if (!isWord(data->tree, cur)) {
			if (isWord(data->treeNames, cur)) {
				printf("A name has been found");
				prev = "he";
			} else if (isWord(data->treePlace, cur)) {
				prev = "it";
			} else {
				char* tmp = fixWord(to_lower(cur), data->map, to_lower(prev));
				correct_typos(entry, cur, tmp, prev);

				free(tmp);
			}
		} else {
			char* t = fixWord(to_lower(cur), data->map, to_lower(prev));
			correct_typos(entry, cur, t, prev);

			free(t);
			prev = cur;
		}

		free(r[0]);
		free(r[1]);
		free(r);
	}

	g_free(current_text);
}



void correct_typos(GtkEntry *entry, const char *old, char *new, const char* prev) {
	const gchar *text = gtk_entry_get_text(entry);
	char *pos = strstr(text, old);

	if (pos != NULL) {

		printf("PREV IS: %s\n", prev);
		if(strcmp(prev, "_") == 0)
		{
			new[0] = toupper(new[0]);
			printf("TOUPPER: %s\n",new);
		}
		// Identify any trailing punctuation
		size_t old_len = strlen(old);
		size_t new_len = strlen(new);
		size_t tail_len = strlen(pos + old_len);
		char punctuation = '\0';

		if (!isalnum(pos[old_len - 1])) {//isalnum checks if it is a letter or number
			punctuation = pos[old_len - 1];
			old_len--; // Adjust old_len to exclude punctuation
		}

		// Ensure enough space for new text
		char *new_text = g_malloc(strlen(text) - old_len + new_len + (punctuation ? 1 : 0) + 1);

		// Copy text before the old word
		strncpy(new_text, text, pos - text);
		new_text[pos - text] = '\0';

		// Append new word
		strcat(new_text, new);

		// Append punctuation if it was present
		if (punctuation) {
			strncat(new_text, &punctuation, 1);
		}

		// Append text after the old word
		strcat(new_text, pos + old_len + (punctuation ? 1 : 0));

		// Block signal handler to prevent recursion
		g_signal_handlers_block_by_func(entry, G_CALLBACK(on_entry_changed), NULL);
		gtk_entry_set_text(entry, new_text);
		g_signal_handlers_unblock_by_func(entry, G_CALLBACK(on_entry_changed), NULL);

		g_free(new_text);
	}
}


int main(int argc, char *argv[]) {
	// Initialization of trees + hashmap
	struct node* tree = buildTreeFromFile("../Tree/text_100k.txt");
	struct node* treeNames = buildTreeFromFile("../Tree/names.txt");
	struct node* treePlace = buildTreeFromFile("../Tree/places2.txt");
	// Init Hash map
	HashMap* map = initHashMap();
	parseWord(map, "./../word_prediction/text.txt");
	sortWordFrequency(map);

	//Init the verb 
	/*
	   const char* filename = "verbs_parsed.txt";
	   Verb* verbs;
	   size_t verb_count = load_verbs(filename, &verbs);
	   if (verb_count == 0) {
	   fprintf(stderr, "Failed to load verbs\n");
	   return 1;
	   }
	   */

	CallbackData data = {
		.tree = tree,
		.treeNames = treeNames,
		.treePlace = treePlace,
		.map = map,
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

