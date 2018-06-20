#include <stdio.h>
#include <stdlib.h>
#include "struct.h"

#define FILENAME_SIZE 256

int main(void) {
    int i, j;
    int **field = NULL;
    FILE *fp = NULL;
    char *filename = NULL;
    
    if((filename = (char *)malloc(FILENAME_SIZE * sizeof(char))) == NULL) {
        exit(1);
    }
    
    printf("input file name: ");
    scanf("%s", filename);
    
    if((fp = fopen(filename, "r")) == NULL){
        exit(1);
    }
    if((field = (int **)malloc(HEIGHT * sizeof(int *))) == NULL) {
        exit(1);
    }
    int value = 0;
    for(i = 0; i < HEIGHT; i++) {
        if((field[i] = malloc(WIDTH * sizeof(int))) == NULL) {
            exit(1);
        }
        for(j = 0; j < WIDTH; j++) {
            fscanf(fp, "%d", &value);
            field[i][j] = value;
        }
    }
    
    fclose(fp);
    
    return 0;
}
