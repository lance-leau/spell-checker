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

static void on_open_clicked(GtkButton *button, gpointer user_data);
static void on_process_clicked(GtkButton *button, gpointer user_data);
static void open_file(GtkWidget *parent_window, GtkTextBuffer *buffer);
void process(const gchar *input_text, gchar **output_text);

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Open and Process Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 400);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to hold the buttons and horizontal pane for text views
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create the Open button
    GtkWidget *open_button = gtk_button_new_with_label("Open");
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), open_button, FALSE, FALSE, 0);

    // Create the Process button
    GtkWidget *process_button = gtk_button_new_with_label("Process");
    g_signal_connect(process_button, "clicked", G_CALLBACK(on_process_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), process_button, FALSE, FALSE, 0);

    // Create a horizontal box to hold the before and after text views
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

    // Create the before text view and add it to a scrolled window
    GtkWidget *scrolled_window_before = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *text_view_before = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window_before), text_view_before);
    gtk_box_pack_start(GTK_BOX(hbox), scrolled_window_before, TRUE, TRUE, 0);

    // Create the after text view and add it to a scrolled window
    GtkWidget *scrolled_window_after = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *text_view_after = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window_after), text_view_after);
    gtk_box_pack_start(GTK_BOX(hbox), scrolled_window_after, TRUE, TRUE, 0);

    // Set the text buffers as user data for the Open and Process button callbacks
    GtkTextBuffer *buffer_before = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_before));
    g_object_set_data(G_OBJECT(open_button), "buffer_before", buffer_before);
    g_object_set_data(G_OBJECT(process_button), "buffer_before", buffer_before);

    GtkTextBuffer *buffer_after = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_after));
    g_object_set_data(G_OBJECT(process_button), "buffer_after", buffer_after);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

static void on_open_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkTextBuffer *buffer_before = g_object_get_data(G_OBJECT(button), "buffer_before");
    open_file(parent_window, buffer_before);
}

static void on_process_clicked(GtkButton *button, gpointer user_data) {
    GtkTextBuffer *buffer_before = g_object_get_data(G_OBJECT(button), "buffer_before");
    GtkTextBuffer *buffer_after = g_object_get_data(G_OBJECT(button), "buffer_after");

    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer_before, &start);
    gtk_text_buffer_get_end_iter(buffer_before, &end);

    gchar *input_text = gtk_text_buffer_get_text(buffer_before, &start, &end, FALSE);
    gchar *output_text = NULL;

    process(input_text, &output_text);

    gtk_text_buffer_set_text(buffer_after, output_text, -1);

    g_free(input_text);
    g_free(output_text);
}

static void open_file(GtkWidget *parent_window, GtkTextBuffer *buffer) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open File",
                                                    GTK_WINDOW(parent_window),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Text files");
    gtk_file_filter_add_pattern(filter, "*.txt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        // Read the content of the file
        gchar *content;
        gsize length;
        if (g_file_get_contents(filename, &content, &length, NULL)) {
            gtk_text_buffer_set_text(buffer, content, length);
            g_free(content);
        } else {
            g_print("Failed to read file: %s\n", filename);
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

char** parseTextToWord(char* input)
{

	char** ret = calloc(1000, sizeof(char*));
	int wordCount = 0;

	char* curr = calloc(WORD_MAX_SIZE, sizeof(char));
	int wordSize = 0;

	for (int i = 0; input[i] != '\0'; i++)
	{
		char ch = input[i];
		if (ch == 44) continue;
		if ('A' <= ch && ch <= 'Z')
		{
			ch = tolower(ch);
		}
		if ('a' <= ch && ch <= 'z')
		{
			curr[wordSize] = ch;
			wordSize++;
		}
		else
		{
			if (wordSize > 0)
			{
				curr[wordSize] = '\0';
				ret[wordCount] = curr;
				wordCount++;
				curr = calloc(WORD_MAX_SIZE, sizeof(char));
				wordSize = 0;
			}
		}
	}

	if (wordSize > 0)
	{
		curr[wordSize] = '\0';
		ret[wordCount] = curr;
		wordCount++;
	}
	else
	{
		free(curr);
	}

	return ret;
}

void process(const gchar *input_text, gchar **output_text) {
    struct node* tree = initTree();

    FILE* file;
    char line[256];
    file = fopen("../Tree/text_100k.txt", "r");

    if (file == NULL) {
        printf("file can't be opened \n");
        *output_text = g_strdup("Error: File cannot be opened.");
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n\r")] = '\0';
        addWord(tree, line);
    }
    fclose(file);

    // Init Hash map
    HashMap* map = initHashMap();
    parseWord(map, "../word_prediction/text3.txt");
    sortWordFrequency(map);
    sortHashMap(map);

    // Parse input text to words
    char** wordArr = parseTextToWord(input_text);
    char* prev = "_";

    printf("input text: %s\n", input_text);

    // Array to hold processed words
    char** ret = calloc(1000, sizeof(char*));

    for (int i = 0; wordArr[i] != NULL; i++) {
        if (!isWord(tree, wordArr[i])) {
            HashMapEntry* entry = findEntry(map, prev);
            if (entry != NULL) {
                int min = 100;
                char* currBest = NULL;
                for (int j = 0; j < entry->followerCount; j++) {
                    int soundSame = isEqual(entry->followers[j].word, wordArr[i]);
                    int dist = distance(entry->followers[j].word, wordArr[i]);
                    if (soundSame == 1 && entry->followers[j].word[0] == wordArr[i][0] && dist < 3) {
                        if (dist < min) {
                            min = dist;
                            currBest = entry->followers[j].word;
                        }
                    }
                }
                if (currBest != NULL) {
                    printf("Fixed <%s> to <%s>\n", wordArr[i], currBest);
                    ret[i] = strdup(currBest);
                    prev = currBest;
                } else {
                    printf("currBest is NULL\n");
                    ret[i] = strdup(wordArr[i]); // Fallback to original word
                    prev = wordArr[i];
                }
            } else {
                printf("no entry found for %s\n", wordArr[i]);
                ret[i] = strdup(wordArr[i]); // Fallback to original word
                prev = wordArr[i];
            }
        } else {
            ret[i] = strdup(wordArr[i]);
            prev = wordArr[i];
        }
    }

    // Combine the processed words into a single string
    GString *result = g_string_new("");
    for (int i = 0; ret[i] != NULL; i++) {
        g_string_append(result, ret[i]);
        g_string_append(result, " ");
        free(ret[i]);
    }

    *output_text = g_string_free(result, FALSE);

    free(ret);

    // Free the word array
    for (int i = 0; wordArr[i] != NULL; i++) {
        free(wordArr[i]);
    }
    free(wordArr);
}

