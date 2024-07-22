#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Verb structure to hold conjugations
typedef struct {
    char base[8521];
    char present_singular[8521];
    char present_participle[8521];
    char past[8521];
    char past_participle[8521];
} Verb;

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
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%s",
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
void correct_verb_form(char *subject, char *verb, char *corrected_sentence, const Verb* verbs, size_t verb_count) {
    const Verb* verb_entry = find_verb(verb, verbs, verb_count);
    if (verb_entry) {
        if (is_pronoun(subject)) {
            if (!is_past_tense(verb, verb_entry)) {
                sprintf(corrected_sentence, "%s %s", subject, verb_entry->present_singular);
            } else {
                sprintf(corrected_sentence, "%s %s", subject, verb);
            }
        } else {
            sprintf(corrected_sentence, "%s %s", subject, verb);
        }
    } else {
        // If verb not found in dictionary, leave it as it is
        sprintf(corrected_sentence, "%s %s", subject, verb);
    }
}

int main() {
    const char* filename = "verbs_parsed.txt";
    Verb* verbs;
    size_t verb_count = load_verbs(filename, &verbs);
    if (verb_count == 0) {
        fprintf(stderr, "Failed to load verbs\n");
        return 1;
    }

    char sentence[] = "he play";
    char corrected_sentence[100];

    char subject[50], verb[50];
    sscanf(sentence, "%s %s", subject, verb);

    correct_verb_form(subject, verb, corrected_sentence, verbs, verb_count);

    printf("Corrected sentence: %s\n", corrected_sentence);

    free(verbs);
    return 0;
}


