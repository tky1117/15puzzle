#include <stdio.h>
#include <stdlib.h>
#include "search.h"

#define FILENAME_SIZE 256

int main(void) {
    int i, j;
    FIELD initialField = 0x0;
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
    
    FIELD value = 0x0;
    
    for(i = 0; i < HEIGHT; i++) {
        for(j = 0; j < WIDTH; j++) {
            initialField = initialField << 4;
            fscanf(fp, "%x", &value);
            initialField += value;
        }
    }
    
    fclose(fp);
    
    NODE *node = AstarSearch(initialField);
    printf("g_cost: %d\n", node -> g_cost);
    showPath(node);
    
    return 0;
}
