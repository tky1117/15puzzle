#include <stdio.h>
#include <stdlib.h>
#include "node.h"

#define CLOSED_LIST_TABLE_MAX (20 * 4)
#define OPEN_LIST_TABLE_MAX (45 + CLOSED_LIST_TABLE_MAX)

struct LIST{
    struct LIST *prevList;
    struct LIST *nextList;
    NODE *node;
};

typedef struct LIST LIST;

void initList();
LIST *makeNewList(NODE *node);
LIST *isIncludedInOpenList(NODE *node);
LIST *isIncludedInClosedList(NODE *node);
LIST **insertToList(LIST **listTable, NODE *node, int cost);
void insertToOpenList(NODE *node);
void insertToClosedList(NODE *node);
NODE *popFromOpenList();
void removeList(LIST *list);

static LIST **openListTable = NULL;
static LIST **closedListTable = NULL;

void initList() {
    int i;
    
    if((openListTable = (LIST **)malloc(OPEN_LIST_TABLE_MAX * sizeof(LIST *))) == NULL) {
        exit(1);
    }
    
    for(i = 0; i < OPEN_LIST_TABLE_MAX; i++) {
        openListTable[i] = NULL;
    }
    
    if((closedListTable = (LIST **)malloc(CLOSED_LIST_TABLE_MAX * sizeof(LIST *))) == NULL) {
        exit(1);
    }
    
    for(i = 0; i < CLOSED_LIST_TABLE_MAX; i++) {
        closedListTable[i] = NULL;
    }
}

LIST *makeNewList(NODE *node) {
    LIST *newList = NULL;
    
    if((newList = (LIST *)malloc(sizeof(LIST))) == NULL) {
        exit(1);
    }
    
    newList -> prevList = NULL;
    newList -> nextList = NULL;
    newList -> node = node;
    
    return newList;
}

LIST *isIncludedInOpenList(NODE *node) {
    int i;
    LIST *tmpOpenList = NULL;
    
    for(i = 0; i < OPEN_LIST_TABLE_MAX; i++) {
        tmpOpenList = openListTable[i];
        
        if(openListTable[i] == NULL) continue;
        
        do {
            if(isSameField(tmpOpenList -> node -> field, node -> field)) {
                return tmpOpenList;
            }
            tmpOpenList = tmpOpenList -> nextList;
        } while(tmpOpenList != openListTable[i]);
    }
    
    return NULL;
}

LIST *isIncludedInClosedList(NODE *node) {
    int i;
    LIST *tmpClosedList = NULL;
    
    if(closedListTable[node -> h_cost] == NULL) {
        return NULL;
    }
    
    tmpClosedList = closedListTable[node -> h_cost];
    
    do {
        if(isSameField(tmpClosedList -> node -> field, node -> field)) {
            return tmpClosedList;
        }
        tmpClosedList = tmpClosedList -> nextList;
    } while(tmpClosedList != closedListTable[node -> h_cost]);
    
    return NULL;
}

LIST **insertToList(LIST **listTable, NODE *node, int cost) {
    LIST *newList = makeNewList(node);
    LIST *tmpTailList = NULL;
    
    if(listTable[cost] == NULL) {
        listTable[cost] = newList;
        newList -> nextList = listTable[cost];
        newList -> prevList = listTable[cost];
        
        return listTable;
    }
    
    tmpTailList = listTable[cost] -> prevList;
    tmpTailList -> nextList = newList;
    newList -> prevList = tmpTailList;
    newList -> nextList = listTable[cost];
    listTable[cost] -> prevList = newList;
    
    return listTable;
}


void insertToOpenList(NODE *node) {
    openListTable = insertToList(openListTable, node, node -> f_cost);
}

void insertToClosedList(NODE *node) {
    closedListTable = insertToList(closedListTable, node, node -> h_cost);
}

NODE *popFromOpenList() {
    int i;
    LIST *tmpOpenList = NULL;
    NODE *node = NULL;
    
    for(i = 0; i < OPEN_LIST_TABLE_MAX; i++) {
        tmpOpenList = openListTable[i];
        
        if(openListTable[i] == NULL) continue;
        
        node = openListTable[i] -> node;
        
        if(openListTable[i] -> nextList == openListTable[i]) {
            openListTable[i] = NULL;
            
            return node;
        }
        
        LIST *tmpHeadList = openListTable[i] -> nextList;
        LIST *tmpTailList = openListTable[i] -> prevList;
        
        tmpHeadList -> prevList = tmpTailList;
        tmpTailList -> nextList = tmpHeadList;
        openListTable[i] = tmpHeadList;
        
        return node;
    }
    
    return NULL;
}

void removeList(LIST *list) {
    LIST *tmpPrevList = list -> prevList;
    LIST *tmpNextList = list -> nextList;
    
    free(list -> node);
    free(list);
    
    if(tmpPrevList == NULL) {
        return;
    }
    
    tmpPrevList -> nextList = tmpNextList;
    tmpNextList -> prevList = tmpPrevList;
}
