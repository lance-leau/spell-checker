
#pragma once

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

int load_verbs(const char *filename, Verb **verbs);
const Verb* find_verb(const char* word, const Verb* verbs, size_t verb_count);
int is_pronoun(const char *word);
int is_past_tense(const char* word, const Verb* verb);
void correct_verb_form(char *subject, char *verb, char *corrected_sentence, const Verb* verbs, size_t verb_count);
