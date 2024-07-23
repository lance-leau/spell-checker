#include "verb.h"

// Function to load verbs from file
int load_verbs(const char *filename, Verb **verbs) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        return 0;
    }

    size_t capacity = 10;
    *verbs = malloc(capacity * sizeof(Verb));
    if (!*verbs) {
        perror("Unable to allocate memory");
        fclose(file);
        return 0;
    }

    size_t count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (count == capacity) {
            capacity *= 2;
            *verbs = realloc(*verbs, capacity * sizeof(Verb));
            if (!*verbs) {
                perror("Unable to reallocate memory");
                fclose(file);
                return 0;
            }
        }

        Verb *verb = &(*verbs)[count];
        verb->base = malloc(50 * sizeof(char));
        verb->present_singular = malloc(50 * sizeof(char));
        verb->present_participle = malloc(50 * sizeof(char));
        verb->past = malloc(50 * sizeof(char));
        verb->past_participle = malloc(50 * sizeof(char));

        sscanf(line, "%49[^,],%49[^,],%49[^,],%49[^,],%49s",
               verb->base, verb->present_singular, verb->present_participle, verb->past, verb->past_participle);
        count++;
    }

    fclose(file);
    return count;
}

// Function to find the verb conjugation
const Verb* find_verb(const char* word, const Verb* verbs, size_t verb_count) {
    for (size_t i = 0; i < verb_count; i++) {
        if (strcmp(word, verbs[i].base) == 0 ||
            strcmp(word, verbs[i].present_singular) == 0 ||
            strcmp(word, verbs[i].present_participle) == 0 ||
            strcmp(word, verbs[i].past) == 0 ||
            strcmp(word, verbs[i].past_participle) == 0) {
            return &verbs[i];
        }
    }
    return NULL;
}

// Function to check if the word is a pronoun
int is_pronoun(const char *word) {
    const char *pronouns[] = {"he", "she", "it", NULL};
    for (int i = 0; pronouns[i] != NULL; i++) {
        if (strcmp(word, pronouns[i]) == 0) return 1;
    }
    return 0;
}

// Function to determine if a verb is in past tense
int is_past_tense(const char* word, const Verb* verb) {
    return strcmp(word, verb->past) == 0 || strcmp(word, verb->past_participle) == 0;
}

// Function to correct the verb form
char* correct_verb_form(const char *prev, const char *cur, const Verb* verbs, size_t verb_count) {
    const Verb* verb_entry = find_verb(cur, verbs, verb_count);
    char* corrected_verb = malloc(50 * sizeof(char));
    if (!corrected_verb) {
        perror("Unable to allocate memory");
        return NULL;
    }

    if (verb_entry) {
        if (is_pronoun(prev)) {
            if (!is_past_tense(cur, verb_entry)) {
                strcpy(corrected_verb, verb_entry->present_singular);
            } else {
                strcpy(corrected_verb, cur);
            }
        } else {
            strcpy(corrected_verb, cur);
        }
    } else {
        // If verb not found in dictionary, return it as is
        strcpy(corrected_verb, cur);
    }

    return corrected_verb;
}
/*
int main() {
    const char* filename = "verbs_parsed.txt";
    Verb* verbs;
    size_t verb_count = load_verbs(filename, &verbs);
    if (verb_count == 0) {
        fprintf(stderr, "Failed to load verbs\n");
        return 1;
    }

    const char* prev = "he";
    const char* cur = "play";

    char* corrected_verb = correct_verb_form(prev, cur, verbs, verb_count);
    if (corrected_verb) {
        printf("Corrected verb: %s\n", corrected_verb);
        free(corrected_verb);
    }

    for (size_t i = 0; i < verb_count; i++) {
        free(verbs[i].base);
        free(verbs[i].present_singular);
        free(verbs[i].present_participle);
        free(verbs[i].past);
        free(verbs[i].past_participle);
    }
    free(verbs);
    
    return 0;
}*/
