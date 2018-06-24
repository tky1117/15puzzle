#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"

//#define DEBUG
void initAll();
void initAstarSearch();
void initManhattanDistance();
void initList();
NODE *AstarSearch(FIELD initialField);
int calcManhattanDistance(int value, int row, int column);
int calcH(FIELD field);
void expand(NODE *node);

static clock_t start, end;
static clock_t totalTime = 0;
static POSITION **move = NULL;
static int count = 0;
static int **manhattanDistance = NULL;

void initAll() {
    initManhattanDistance();
    initList();
    initAstarSearch();
}

void initManhattanDistance() {
    int i, j;
    int row0, column0, row1, column1;
    int distanceOfRow, distanceOfColumn;
    
    if((manhattanDistance = (int **)malloc(HEIGHT * WIDTH * sizeof(int *))) == NULL) {
        exit(1);
    }
    for(i = 0; i < HEIGHT * WIDTH; i++) {
        if((manhattanDistance[i] = (int *)malloc(HEIGHT * WIDTH * sizeof(int))) == NULL) {
            exit(1);
        }
        for(j = 0; j < HEIGHT * WIDTH; j++) {
            row0 = i / HEIGHT;
            column0 = i % WIDTH;
            row1 = j / HEIGHT;
            column1 = j % WIDTH;
            
            distanceOfRow = row0 - row1;
            distanceOfColumn = column0 - column1;
            
            if(distanceOfRow < 0) distanceOfRow = -distanceOfRow;
            if(distanceOfColumn < 0) distanceOfColumn = -distanceOfColumn;
            
            manhattanDistance[i][j] = distanceOfRow + distanceOfColumn;
        }
    }
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
        
        expand(node);
        
        insertToClosedList(node);
        
#ifdef DEBUG
        showOpenList();
        showClosedList();
#endif //DEBUG
    }
    return NULL;
}

int calcManhattanDistance(int value, int row, int column) {
    int correctRow = value / HEIGHT;
    int correctColumn = value % WIDTH;
    int distanceOfRow = row - correctRow;
    int distanceOfColumn = column - correctColumn;
    
    if(distanceOfRow < 0) distanceOfRow = -distanceOfRow;
    if(distanceOfColumn < 0) distanceOfColumn = -distanceOfColumn;
    
    return distanceOfRow + distanceOfColumn;
}

int calcH(FIELD field) {
    int i, j;
    int totalDistance = 0;
    FIELD byteMask = 0xf;
    FIELD value = 0;

    for(i = HEIGHT - 1; i >= 0; i--) {
        for(j = WIDTH - 1 ; j >= 0; j--) {
            value = field & byteMask;
            if(value) {
                //valueが0でない場合
                totalDistance += manhattanDistance[value][i * HEIGHT + j];
            }
            field = field >> 4;
        }
    }
    
    return totalDistance;
}

int calcHDelta(NODE *node) {
    POSITION *positionOfSpace = node -> positionOfSpace;
    POSITION *parentPositionOfSpace = node -> parentNode -> positionOfSpace;
    FIELD field = node -> field;
    int row = positionOfSpace -> row;
    int column = positionOfSpace -> column;
    int parentRow = parentPositionOfSpace -> row;
    int parentColumn = parentPositionOfSpace -> column;
    FIELD value = getValue(field, parentRow, parentColumn);
    
    int delta = manhattanDistance[value][parentRow * HEIGHT + parentColumn] - manhattanDistance[value][row * HEIGHT + column];
    
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
        
        FIELD zeroByteMask = 0xf;
        FIELD value = getValue(field, newRow, newColumn);

        zeroByteMask = zeroByteMask << shiftTo(newRow, newColumn);
        zeroByteMask = ~zeroByteMask;
        value = value << shiftTo(row, column);
        FIELD newField = (node -> field | value) & zeroByteMask;
        NODE *newNode = makeNewNode(newField);
        newNode -> parentNode = node;
        newNode -> positionOfSpace = makeNewPosition(newRow, newColumn);
        newNode -> g_cost = node -> g_cost + 1;
        newNode -> h_cost = node -> h_cost + calcHDelta(newNode);
        newNode -> f_cost = newNode -> g_cost + newNode -> h_cost;

        LIST *removableOpenList = NULL;
        /*isIncludedInOpenList(newNode);
        
        if(removableOpenList != NULL) {
            if(newNode -> f_cost < removableOpenList -> node -> f_cost) {
                //新しいnodeの方がコストが小さい
                removeList(removableOpenList);
            } else {
                continue;
            }
        }
        */
        
        start = clock();
        LIST *removableClosedList = isIncludedInClosedList(newNode);
        end = clock();
        totalTime += end - start;
        
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

int getCount() {
    return count;
}
