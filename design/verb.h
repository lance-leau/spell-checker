
#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Verb structure to hold conjugations
typedef struct {
    char* base;
    char* present_singular;
    char* present_participle;
    char* past;
    char* past_participle;
} Verb;

int load_verbs(const char *filename, Verb **verbs);
const Verb* find_verb(const char* word, const Verb* verbs, size_t verb_count);
int is_pronoun(const char *word);
int is_past_tense(const char* word, const Verb* verb);
char* correct_verb_form(const char *prev, const char *cur, const Verb* verbs, size_t verb_count);
