#include <stdio.h>
#include "node.h"

LIST *makeNewList(NODE *node);
LIST *isIncludedInList(LIST *listHead, NODE *node);
LIST *isIncludedInOpenList(NODE *node);
LIST *isIncludedInClosedList(NODE *node);
LIST *insertToList(LIST *listHead, NODE *node);
void *insertToOpenList(NODE *node);
void *insertToClosedList(NODE *node);
NODE *popFromList(LIST **listHeadPtr);
NODE *popFromOpenList();
NODE *popFromClosedList();

static LIST *openListHead = NULL;
static LIST *closedListHead = NULL;

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

LIST *isIncludedInList(LIST *listHead, NODE *node) {
    LIST *tmpList = listHead;
    
    if(tmpList == NULL) return NULL;
    
    do {
        if(isSameField(tmpList -> node -> field, node -> field)) {
            return tmpList;
        }
        tmpList = tmpList -> nextList;
    } while(tmpList != listHead);
    
    return NULL;
}

LIST *isIncludedInOpenList(NODE *node) {
    return isIncludedInList(openListHead, node);
}

LIST *isIncludedInClosedList(NODE *node) {
    return isIncludedInList(closedListHead, node);
}

LIST *insertToList(LIST *listHead, NODE *node) {
    LIST *newList = makeNewList(node);
    LIST *tmpList = listHead;
    LIST *tmpPrevList = NULL;

    if(listHead == NULL) {
        listHead = newList;
        newList -> nextList = listHead;
        newList -> prevList = listHead;
        
        return listHead;
    }
    
    if(node -> f_cost < listHead -> node -> f_cost) { //listの先頭に挿入
        tmpPrevList = listHead -> prevList;
        tmpPrevList -> nextList = newList;
        newList -> prevList = tmpPrevList;
        newList -> nextList = listHead;
        listHead -> prevList = newList;
        listHead = newList;
        
        return listHead;
    }
    
    tmpList = tmpList -> nextList;
    
    while((tmpList != listHead) && (tmpList -> node -> f_cost < node -> f_cost)) {
        tmpList = tmpList -> nextList;
    }
    
    tmpPrevList = tmpList -> prevList;
    tmpPrevList -> nextList = newList;
    newList -> prevList = tmpPrevList;
    newList -> nextList = tmpList;
    tmpList -> prevList = newList;
    
    return listHead;
}

void *insertToOpenList(NODE *node) {
    openListHead = insertToList(openListHead, node);
}

void *insertToClosedList(NODE *node) {
    closedListHead = insertToList(closedListHead, node);
}

NODE *popFromList(LIST **listHeadPtr) {
    LIST *listHead = *listHeadPtr;
    NODE *node = NULL;
    
    if(listHead == NULL) {
        return NULL;
    }
    
    node = listHead -> node;
    
    if(listHead -> nextList == listHead) {
        listHead = NULL;
        
        return node;
    }
    
    LIST *tmpNextList = listHead -> nextList;
    LIST *tmpTailList = listHead -> prevList;
    
    tmpNextList -> prevList = tmpTailList;
    tmpTailList -> nextList = tmpNextList;
    
    *listHeadPtr = tmpNextList;

    return node;
}

NODE *popFromOpenList() {
    return popFromList(&openListHead);
}

NODE *popFromClosedList() {
    return popFromList(&closedListHead);
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

void showList(LIST *listHead) {
    LIST *tmpList = listHead;
    
    if(tmpList == NULL) return;
    
    do {
        showField(tmpList -> node -> field);
        printf("fcost: %d\n", tmpList -> node -> f_cost);
        printf("---------------\n");
        tmpList = tmpList -> nextList;
    } while(tmpList != listHead);
}

void showOpenList() {
    printf("/*==in openList==\n");
    showList(openListHead);
    printf("==in openList==*/\n");
}

void showClosedList() {
    printf("/*==in closedList==\n");
    showList(closedListHead);
    printf("==in closedList==*/\n");
}
