# Spell Checker

## Project Overview

The goal of this project is to create a spell checker that can correct typos and provide context-based corrections, similar to tools like Reverso, QuillBot and Grammarly. The spell checker reads a text file, processes each word, and generates a corrected text file. The corrections are based on a combination of Damerau-Levenshtein distance, Metaphone distance, and n-gram analysis.

## Table of Contents

- [Project Overview](#project-overview)
- [Project Structure](#project-structure)
- [Data Structures and Algorithms](#data-structures-and-algorithms)
- [References](#references)

## Project Structure

1. **Data Loading**: Load a pre-processed tree of the 10,000 most common words of the English language into a general tree structure for optimized search. Load a pre-processed n-gram of the most common sentences.
2. **Spell Checking**: For each word in the input text:
    - If the word is correctly spelled and coherent in the context of the sentence, move on to the next word.
    - Otherwise, find the most likely words given the context, and check which is the closest using Damerau-Levenshtein distance and Metaphone distance.
    - Replace the misspelled word with the corrected word.

## Data Structures and Algorithms

### Tree Data Structure
We created our own tree class to store the 10,000 most common English words. This structure allows for efficient word lookup and validation.

### N-gram Model
We generated an n-gram model from a corpus of books to understand word sequences and context. The model helps in predicting the next word and provides context-based corrections.

### Hash Map and Histogram
A hash map and histogram are used to track word frequencies and their co-occurrences. This data helps in determining the most likely corrections based on context.

### Damerau-Levenshtein Distance
This algorithm calculates the minimum number of operations (insertions, deletions, substitutions, or transpositions) required to transform one word into another. It is used to find the closest word to a misspelled word.

### Metaphone Distance
Metaphone is a phonetic algorithm for indexing words by their sound when pronounced in English. It is used to find phonetically similar words.

## References

- [Damerau-Levenshtein Distance](https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance)
- [Metaphone Algorithm](https://en.wikipedia.org/wiki/Metaphone)
- [N-gram Models](https://en.wikipedia.org/wiki/N-gram)
