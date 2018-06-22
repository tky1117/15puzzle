#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"

//#define DEBUG
void initAll();
void initAstarSearch();
NODE *AstarSearch(FIELD initialField);
int calcManhattanDistance(int value, int row, int column);
int calcH(FIELD field);
void expand(NODE *node);

clock_t start, end;
clock_t totalTime = 0;
POSITION **move = NULL;

void initAll() {
    initList();
    initAstarSearch();
}

void initAstarSearch() {
    int direction;
    
    if((move = (POSITION **)malloc(4 * sizeof(POSITION *))) == NULL) {
        exit(1);
    }
    
    for(direction = North; direction <= East; direction++) {
        if((move[direction] = (POSITION *)malloc(2 * sizeof(POSITION))) == NULL) {
            exit(1);
        }
        move[direction] -> row = 0;
        move[direction] -> column = 0;
    }
    
    move[North] -> row = -1;
    move[West] -> column = -1;
    move[South] -> row = 1;
    move[East] -> column = 1;
}

NODE *AstarSearch(FIELD initialField) {
    initAll();
    
    NODE *initialNode = makeNewNode(initialField);
    initialNode -> g_cost = 0;
    initialNode -> h_cost = calcH(initialField);
    initialNode -> f_cost = initialNode -> g_cost + initialNode -> h_cost;
    initialNode -> positionOfSpace = getPositionOfSpace(initialField);
    
    
    insertToOpenList(initialNode);
    
    while(1) {
        NODE *node = popFromOpenList();
        
        if(node == NULL) {
            break;
        }
        
        if(isGoal(node)) {
            return node;
        }
        expand(node);//問題
        
        insertToClosedList(node);
        
#ifdef DEBUG
        showOpenList();
        showClosedList();
#endif //DEBUG
    }
    return NULL;
}

//0.6秒
int calcManhattanDistance(int value, int row, int column) {
    int correctRow = value / HEIGHT;
    int correctColumn = value % WIDTH;
    int distanceOfRow = row - correctRow;
    int distanceOfColumn = column - correctColumn;
    
    if(distanceOfRow < 0) distanceOfRow = -distanceOfRow;
    if(distanceOfColumn < 0) distanceOfColumn = -distanceOfColumn;
    
    return distanceOfRow + distanceOfColumn;
}
//->0.14
int calcH(FIELD field) {
    int i, j;
    int totalDistance = 0;
    FIELD mask = 0xf;
    FIELD value = 0;

    for(i = HEIGHT - 1; i >= 0; i--) {
        for(j = WIDTH - 1 ; j >= 0; j--) {
            value = field & mask;
            if(value) {
                //valueが0でない場合
                totalDistance += calcManhattanDistance(value, i, j);
            }
            field = field >> 4;
        }
    }
    
    return totalDistance;
}

//->0.12
int calcHDelta(NODE *node) {
    POSITION *positionOfSpace = node -> positionOfSpace;
    POSITION *parentPositionOfSpace = node -> parentNode -> positionOfSpace;
    FIELD field = node -> field;
    int row = positionOfSpace -> row;
    int column = positionOfSpace -> column;
    int parentRow = parentPositionOfSpace -> row;
    int parentColumn = parentPositionOfSpace -> column;
    //
    FIELD value = getValue(field, parentRow, parentColumn);
    
    int delta = calcManhattanDistance(value, parentRow, parentColumn) - calcManhattanDistance(value, row, column);
    
    return delta;
}

void expand(NODE *node) {
    DIRECTION direction;
    int row = node -> positionOfSpace -> row;
    int column = node -> positionOfSpace -> column;
    FIELD field = node -> field;
    
    for(direction = North; direction <= East; direction++) {
        int newRow = row + move[direction] -> row;
        int newColumn = column + move[direction] -> column;
        POSITION *newPosition = makeNewPosition(newRow, newColumn);
        
        if(!isInRange(newPosition)) {
            continue;
        }
        
        FIELD zeroMask = 0xf;
        FIELD value = getValue(field, newRow, newColumn);

        zeroMask = zeroMask << shiftTo(newRow, newColumn);
        zeroMask = ~zeroMask;
        
        value = value << shiftTo(row, column);
        
        FIELD newField = (node -> field | value) & zeroMask;
        NODE *newNode = makeNewNode(newField);
        newNode -> parentNode = node;
        newNode -> positionOfSpace = makeNewPosition(newRow, newColumn);
        newNode -> g_cost = node -> g_cost + 1;
        //here
        start = clock();
        newNode -> h_cost = node -> h_cost + calcHDelta(newNode);
        end = clock();
        totalTime += end - start;
        newNode -> f_cost = newNode -> g_cost + newNode -> h_cost;
        
        
        LIST *removableOpenList = isIncludedInOpenList(newNode);
        
        
        if(removableOpenList != NULL) {
            if(newNode -> f_cost < removableOpenList -> node -> f_cost) {
                //新しいnodeの方がコストが小さい
                removeList(removableOpenList);
            } else {
                continue;
            }
        }
        
        LIST *removableClosedList = isIncludedInClosedList(newNode);
        
        if(removableClosedList != NULL) {
            if(newNode -> f_cost < removableClosedList -> node -> f_cost) {
                //新しいnodeの方がコストが小さい
                removeList(removableClosedList);
            } else {
                continue;
            }
        }
        insertToOpenList(newNode);
    }
}

double getTotalTime() {
    return (double)totalTime / CLOCKS_PER_SEC;
}

