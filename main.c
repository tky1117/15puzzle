#include <stdio.h>
#include <stdlib.h>

int main(void) {
    FILE *fp = NULL;
    char *filename = NULL;
    
    printf("input file name: ");
    scanf("%s", filename);
    if((fp = fopen(filename, "r")) == NULL){
        exit(1);
    }
    fclose(fp);
    
    return 0;
}
