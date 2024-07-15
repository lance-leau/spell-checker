#ifndef METAPHONE_H
#define METAPHONE_H

#include <stddef.h>

char* removeChar(char* s, size_t index);
char* transformChar(char* s, size_t index, char replace);
char* removeAdj(char* s);
char* handleInitialModifications(char* s);
char* handleCharacterTransformations(char* s);
char* dropVowels(char* s);
char* crop(char* s);
int isEqual(char* a, char* b);

#endif // METAPHONE_H

