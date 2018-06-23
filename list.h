#include <stdio.h>
#include <stdlib.h>
#include "node.h"

#define H_COST_MAX (20 * 4)

#define CLOSED_LIST_TABLE_MAX H_COST_MAX
#define OPEN_LIST_TABLE_MAX (45 + H_COST_MAX)

#define OPEN_LIST_TABLE_INDEX_MAX H_COST_MAX
#define CLOSED_LIST_TABLE_INDEX_MAX 0xfedc

struct LIST {
    struct LIST *prevList;
    struct LIST *nextList;
    NODE *node;
};

typedef struct LIST LIST;

void initList();
void initListTable();
void initListTree();
LIST *makeNewList(NODE *node);
LIST *isIncludedInOpenList(NODE *node);
LIST *isIncludedInClosedList(NODE *node);
LIST ***insertToList(LIST ***listTable, NODE *node, int cost, int index);
void insertToOpenList(NODE *node);
void insertToClosedList(NODE *node);
NODE *popFromOpenList();
void removeList(LIST *list);
int getLengthOfList(LIST *listHead);
int getLengthOfOpenListTable();

static LIST ***openListTable = NULL;
static LIST ***closedListTable = NULL;
static FIELD bitMask = 0xffff;

void initList() {
    initListTable();
}

void initListTable() {
    int i, j;
    
    if((openListTable = (LIST ***)malloc(OPEN_LIST_TABLE_MAX * sizeof(LIST **))) == NULL) {
        exit(1);
    }
    
    for(i = 0; i < OPEN_LIST_TABLE_MAX; i++) {
        if((openListTable[i] = (LIST **)malloc(H_COST_MAX * sizeof(LIST *))) == NULL) {
            exit(1);
        }
        
        for(j = 0; j < OPEN_LIST_TABLE_INDEX_MAX; j++) {
            openListTable[i][j] = NULL;
        }
    }
    
    if((closedListTable = (LIST ***)malloc(CLOSED_LIST_TABLE_MAX * sizeof(LIST **))) == NULL) {
        exit(1);
    }
    
    for(i = 0; i < CLOSED_LIST_TABLE_MAX; i++) {
        if((closedListTable[i] = (LIST **)malloc(CLOSED_LIST_TABLE_INDEX_MAX * sizeof(LIST *))) == NULL) {
            exit(1);
        }
        for(j = 0; j < CLOSED_LIST_TABLE_INDEX_MAX; j++) {
            closedListTable[i][j] = NULL;
        }
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

//遅い
//6.2秒 -> 2.8秒/24.8秒
LIST *isIncludedInOpenList(NODE *node) {
    int i;
    LIST *tmpOpenList = NULL;
    int h_cost = node -> h_cost;
    
    for(i = 0; i < OPEN_LIST_TABLE_MAX; i++) {
        tmpOpenList = openListTable[i][h_cost];
        
        if(openListTable[i][h_cost] == NULL) continue;
        
        do {
            if(isSameField(tmpOpenList -> node -> field, node -> field)) {
                return tmpOpenList;
            }
            tmpOpenList = tmpOpenList -> nextList;
        } while(tmpOpenList != openListTable[i][h_cost]);
    }
    
    return NULL;
}

//遅い
//1.6秒 -> 0.4秒/4.2秒
LIST *isIncludedInClosedList(NODE *node) {
    LIST *tmpClosedList = NULL;
    int h_cost = node -> h_cost;
    int index = node -> field & bitMask;
    
    if(closedListTable[h_cost][index] == NULL) {
        return NULL;
    }
    
    tmpClosedList = closedListTable[h_cost][index];
    
    do {
        if(isSameField(tmpClosedList -> node -> field, node -> field)) {
            return tmpClosedList;
        }
        tmpClosedList = tmpClosedList -> nextList;
    } while(tmpClosedList != closedListTable[h_cost][index]);
    
    return NULL;
}

LIST ***insertToList(LIST ***listTable, NODE *node, int cost, int index) {
    LIST *newList = makeNewList(node);
    LIST *tmpTailList = NULL;
    
    if(listTable[cost][index] == NULL) {
        listTable[cost][index] = newList;
        newList -> nextList = listTable[cost][index];
        newList -> prevList = listTable[cost][index];
        
        return listTable;
    }
    
    tmpTailList = listTable[cost][index] -> prevList;
    tmpTailList -> nextList = newList;
    newList -> prevList = tmpTailList;
    newList -> nextList = listTable[cost][index];
    listTable[cost][index] -> prevList = newList;
    
    return listTable;
}

//0.05秒
void insertToOpenList(NODE *node) {
    openListTable = insertToList(openListTable, node, node -> f_cost, node -> h_cost);
}

//0.03秒
void insertToClosedList(NODE *node) {
    closedListTable = insertToList(closedListTable, node, node -> h_cost, node -> field & bitMask);
}

NODE *popFromOpenList() {
    int i, j;
    NODE *node = NULL;
    
    for(i = 0; i < OPEN_LIST_TABLE_MAX; i++) {
        for(j = 0; j < OPEN_LIST_TABLE_INDEX_MAX; j++) {
            if(openListTable[i][j] == NULL) continue;
            
            node = openListTable[i][j] -> node;
            
            if(openListTable[i][j] -> nextList == openListTable[i][j]) {
                openListTable[i][j] = NULL;
                
                return node;
            }
            
            LIST *tmpHeadList = openListTable[i][j] -> nextList;
            LIST *tmpTailList = openListTable[i][j] -> prevList;
            
            tmpHeadList -> prevList = tmpTailList;
            tmpTailList -> nextList = tmpHeadList;
            openListTable[i][j] = tmpHeadList;
            
            return node;
        }
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

int getLengthOfList(LIST *listHead) {
    LIST *tmpList = NULL;
    int length = 0;
    if(listHead == NULL) {
        return 0;
    }
    tmpList = listHead;
    do {
        length++;
        tmpList = tmpList -> nextList;
    } while(tmpList != listHead);
    
    return length;
}

int getLenthOfOpenListTable() {
    int i, j;
    int length = 0;
    
    for(i = 0; i < OPEN_LIST_TABLE_MAX; i++) {
        for(j = 0; j < OPEN_LIST_TABLE_INDEX_MAX; j++) {
            length += getLengthOfList(openListTable[i][j]);
        }
    }
    
    return length;
}
