#include <gtk/gtk.h>

// Function to correct typos in the text
void correct_typos(char *text) {
    // Example correction: replace 'teh' with 'the'
    char *pos = strstr(text, "teh");
    //printf("%s", text);
    if (pos != NULL) {
        strncpy(pos, "the", 3);
    }
}

// Signal handler for the 'changed' signal
void on_entry_changed(GtkEntry *entry, gpointer user_data) {
    // Get the current text from the entry
    const gchar *current_text = gtk_entry_get_text(entry);
    
    // Create a mutable copy of the text
    char *text = g_strdup(current_text);
    
    // Correct typos in the text
    correct_typos(text);
    
    // Set the corrected text back into the entry
    // Check if the text was actually changed to avoid infinite loop
    if (g_strcmp0(current_text, text) != 0) {
        g_signal_handlers_block_by_func(entry, G_CALLBACK(on_entry_changed), NULL);
        gtk_entry_set_text(entry, text);
        g_signal_handlers_unblock_by_func(entry, G_CALLBACK(on_entry_changed), NULL);
    }
    
    // Free the copied text
    g_free(text);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    // Create a new window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Typo Correction");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    
    // Create a new entry widget
    GtkWidget *entry = gtk_entry_new();
    
    // Connect the 'changed' signal to the signal handler
    g_signal_connect(entry, "changed", G_CALLBACK(on_entry_changed), NULL);
    
    // Add the entry widget to the window
    gtk_container_add(GTK_CONTAINER(window), entry);
    
    // Show all widgets in the window
    gtk_widget_show_all(window);
    
    // Connect the 'destroy' signal to exit the GTK main loop
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Run the GTK main loop
    gtk_main();
    
    return 0;
}

