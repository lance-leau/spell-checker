#include "levenshtein.h"

// Function to find the minimum of three numbers
int min3(int a, int b, int c) {
    if (a < b && a < c) return a;
    else if (b < c) return b;
    else return c;
}

// Function to find the minimum of two numbers
int min2(int a, int b) {
    return (a < b) ? a : b;
}

// Function to calculate the optimal string alignment (Levenshtein) distance with transpositions
int distance(const char* s1, const char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int i, j;

    // Create a table to store the results of subproblems
    int** dp = (int**)malloc((len1 + 1) * sizeof(int*));
    for (i = 0; i <= len1; i++) {
        dp[i] = (int*)malloc((len2 + 1) * sizeof(int));
    }

    // Initialize the table
    for (i = 0; i <= len1; i++) {
        dp[i][0] = i;
    }
    for (j = 0; j <= len2; j++) {
        dp[0][j] = j;
    }

    // Populate the table using dynamic programming
    for (i = 1; i <= len1; i++) {
        for (j = 1; j <= len2; j++) {
            if (s1[i-1] == s2[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                int insert = dp[i][j-1];
                int del = dp[i-1][j];
                int replace = dp[i-1][j-1];
                dp[i][j] = 1 + min3(insert, del, replace);

                // Check for transposition
                if (i > 1 && j > 1 && s1[i-1] == s2[j-2] && s1[i-2] == s2[j-1]) {
                    dp[i][j] = min2(dp[i][j], dp[i-2][j-2] + 1);
                }
            }
        }
    }

    // Get the result from the table
    int result = dp[len1][len2];

    // Free allocated memory
    for (i = 0; i <= len1; i++) {
        free(dp[i]);
    }
    free(dp);

    return result;
}

int main() {
    printf("%d\n", distance("road", "raod"));
    return 0;
}

