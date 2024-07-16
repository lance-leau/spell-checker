#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../levenshtein/levenshtein.h"

// Function to remove a character at a given index
char* removeChar(char* s, size_t index) {
    size_t n = strlen(s);
    char* result = malloc((n + 1) * sizeof(char));
    if (result == NULL) {
        perror("Unable to allocate memory");
        exit(1);
    }
    strncpy(result, s, index);
    strcpy(result + index, s + index + 1);
    return result;
}

// Function to transform a character at a given index
char* transformChar(char* s, size_t index, char replace) {
    size_t n = strlen(s);
    char* r = malloc((n + 1) * sizeof(char));
    if (r == NULL) {
        perror("Unable to allocate memory");
        exit(1);
    }
    strncpy(r, s, index);
    r[index] = replace;
    strcpy(r + index + 1, s + index + 1);
    return r;
}

// Function to remove duplicate adjacent letters except for 'C'
char* removeAdj(char* s) {
    size_t n = strlen(s);
    char* r = malloc((n + 1) * sizeof(char));
    size_t i = 0, j = 0;

    while (i < n - 1) {
        if (s[i] == s[i + 1] && s[i] != 'c') {
            i++;
        } else {
            r[j++] = s[i++];
        }
    }
    r[j++] = s[i];
    r[j] = '\0';
    return r;
}

// Function to handle initial string modifications based on specific prefixes
char* handleInitialModifications(char* s) {
    char* r = removeAdj(s);
    size_t n = strlen(r);

    if (n < 2) {
        return r;
    }

    // Drop the first letter if the string begins with AE, GN, KN, PN or WR
    if (strncmp(r, "ae", 2) == 0 || strncmp(r, "gn", 2) == 0 ||
        strncmp(r, "kn", 2) == 0 || strncmp(r, "pn", 2) == 0 ||
        strncmp(r, "wr", 2) == 0) {
        char* new_r = removeChar(r, 0);
        free(r);
        r = new_r;
    }

    // Drop B if after M at the end of the string
    n = strlen(r);
    if (n >= 2 && r[n - 1] == 'b' && r[n - 2] == 'm') {
        char* new_r = removeChar(r, n - 1);
        free(r);
        r = new_r;
    }

    return r;
}

// Function to handle specific character transformations
char* handleCharacterTransformations(char* s) {
    size_t n = strlen(s);
    char* r = malloc((n + 1) * sizeof(char));
    strcpy(r, s);

    for (size_t i = 0; i < n; i++) {
        if (r[i] == 'b' && i > 0 && r[i - 1] == 'm' && r[i + 1] == '\0') {
            r = removeChar(r, i);
            i--;
            n--;
        } else if (r[i] == 'c') {
            if (strncmp(&r[i + 1], "ia", 2) == 0 || r[i + 1] == 'h') {
                r = transformChar(r, i, 'x');
            } else if (r[i + 1] == 'i' || r[i + 1] == 'e' || r[i + 1] == 'y') {
                r = transformChar(r, i, 's');
            } else {
                r = transformChar(r, i, 'k');
            }
        } else if (r[i] == 'd') {
            if (strncmp(&r[i + 1], "ge", 2) == 0 || strncmp(&r[i + 1], "gy", 2) == 0 || strncmp(&r[i + 1], "gi", 2) == 0) {
                r = transformChar(r, i, 'j');
            } else {
                r = transformChar(r, i, 't');
            }
        } else if (r[i] == 'g') {
            if (r[i + 1] == 'h' && r[i + 2] != '\0' && strchr("aeiou", r[i + 2]) == NULL) {
                r = removeChar(r, i);
                n--;
                continue;
            } else if ((r[i + 1] == 'n' && r[i + 2] == '\0') || (strncmp(&r[i + 1], "ned", 3) == 0 && r[i + 4] == '\0')) {
                r = removeChar(r, i);
                n--;
                continue;
            } else if ((r[i + 1] == 'i' || r[i + 1] == 'e' || r[i + 1] == 'y') && (i == 0 || r[i - 1] != 'g')) {
                r = transformChar(r, i, 'j');
            } else {
                r = transformChar(r, i, 'k');
            }
        } else if (r[i] == 'h') {
            if ((i > 0 && strchr("aeiou", r[i - 1]) != NULL && (r[i + 1] == '\0' || strchr("aeiou", r[i + 1]) == NULL)) || (i > 0 && strchr("cspgt", r[i - 1]) != NULL)) {
                r = removeChar(r, i);
                n--;
            }
        } else if (r[i] == 'k' && i > 0 && r[i - 1] == 'c') {
            r = removeChar(r, i);
            n--;
        } else if (strncmp(&r[i], "ph", 2) == 0) {
            r = transformChar(r, i, 'f');
            r = removeChar(r, i + 1);
            n--;
        } else if (r[i] == 'q') {
            r = transformChar(r, i, 'k');
        } else if (r[i] == 's' && (strncmp(&r[i + 1], "h", 1) == 0 || strncmp(&r[i + 1], "io", 2) == 0 || strncmp(&r[i + 1], "ia", 2) == 0)) {
            r = transformChar(r, i, 'x');
        } else if (r[i] == 't' && (strncmp(&r[i + 1], "ia", 2) == 0 || strncmp(&r[i + 1], "io", 2) == 0)) {
            r = transformChar(r, i, 'x');
        } else if (strncmp(&r[i], "th", 2) == 0) {
            r = transformChar(r, i, '0');
            r = removeChar(r, i + 1);
            n--;
        } else if (r[i] == 't' && strncmp(&r[i + 1], "ch", 2) == 0) {
            r = removeChar(r, i);
            n--;
        } else if (r[i] == 'v') {
            r = transformChar(r, i, 'f');
        } else if (r[i] == 'w' && (r[i + 1] == '\0' || strchr("aeiou", r[i + 1]) == NULL)) {
            r = removeChar(r, i);
            n--;
        } else if (strncmp(&r[i], "wh", 2) == 0 && i == 0) {
            r = removeChar(r, i + 1);
            n--;
        } else if (r[i] == 'x') {
            if (i == 0) {
                r = transformChar(r, i, 's');
            } else {
                r = transformChar(r, i, 'k');
                char* new_r = malloc((strlen(r) + 2) * sizeof(char));
                strncpy(new_r, r, i + 1);
                new_r[i + 1] = 's';
                strcpy(new_r + i + 2, r + i + 1);
                free(r);
                r = new_r;
                n++;
            }
        } else if (r[i] == 'y' && (r[i + 1] == '\0' || strchr("aeiou", r[i + 1]) == NULL)) {
            r = removeChar(r, i);
            n--;
        } else if (r[i] == 'z') {
            r = transformChar(r, i, 's');
        }
    }

    return r;
}

// Function to drop all vowels unless it is the beginning character
char* dropVowels(char* s) {
    size_t n = strlen(s);
    char* r = malloc((n + 1) * sizeof(char));
    size_t j = 0;

    for (size_t i = 0; i < n; i++) {
        if (i == 0 || strchr("aeiou", s[i]) == NULL) {
            r[j++] = s[i];
        }
    }
    r[j] = '\0';
    return r;
}

// Main function to crop and transform the string
char* crop(char* s) {
    char* r = handleInitialModifications(s);
    char* transformed_r = handleCharacterTransformations(r);
    free(r);
    char* final_r = dropVowels(transformed_r);
    free(transformed_r);
    return final_r;
}

int isEqual(char* a, char* b)
{
        char* ca = crop(a);
        char* cb = crop(b);

        return distance(ca,cb) <= 1 ? 1 : 0;
}
/*
int main() {
    char* s = "keeping";
    char* r = "keeoing";

    if(isEqual(s,r))
	    printf("Nice");
    else
	    printf("Shut up");
    
    return 0;
}
*/
