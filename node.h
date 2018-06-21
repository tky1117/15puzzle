#include "struct.h"

NODE *makeNewNode(FIELD field);

NODE *makeNewNode(FIELD field) {
    int i, j;
    NODE *newNode = NULL;
    
    if((newNode = (NODE *)malloc(sizeof(NODE))) == NULL) {
        exit(1);
    }
    
    newNode -> field = field;
    
    newNode -> field = field;
    newNode -> positionOfSpace = makeNewPosition(-1, -1);
    newNode -> g_cost = 0;
    newNode -> h_cost = 0;
    newNode -> f_cost = 0;
    newNode -> parentNode = NULL;
    
    return newNode;
}
