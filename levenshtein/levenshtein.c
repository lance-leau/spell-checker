#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int min(int a, int b)
{
    return a > b ? b : a;
}

int distance(char* a, char* b)
{
    int n = strlen(a);
    int m = strlen(b);
   
    int** d = malloc((n + 1) * sizeof(int*));
    for (int i = 0; i <= n; i++) {
        d[i] = malloc((m + 1) * sizeof(int));
    }
    int i,j = 0;
    
    for(i = 0;i <= n; i++)
    {
        d[i][0] = i;
    }
    
    for(j = 0; j <= m; j++)
    {
        d[0][j] = j;
    }
    
    for(i = 1; i <= n; i++)
    {
        for(j = 1; j <= m; j++)
        {
            if (a[i-1] == b[j-1]) {
                d[i][j] = d[i-1][j-1];
            } else {
                d[i][j] = 1 + min(d[i-1][j], min(d[i][j-1], d[i-1][j-1]));
            }
        }
    }
    
    return d[n][m];

}

int main() {
    
    char* a = "niche";
    char* b = "chien";
    
    int r = distance(a,b);
    printf("Distance: %d", r);
    

    return 0;
}
