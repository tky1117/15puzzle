#define HEIGHT 4
#define WIDTH 4

typedef unsigned long long FIELD;

typedef struct {
    int row;
    int column;
} POSITION;

struct NODE {
    FIELD field;
    POSITION *positionOfSpace;
    int g_cost;
    int h_cost;
    int f_cost;
    struct NODE *parentNode;
};

typedef struct NODE NODE;

struct LIST {
    struct LIST *prevList;
    struct LIST *nextList;
    NODE *node;
};

typedef struct LIST LIST;

typedef enum{
    North, West, South, East
} DIRECTION;

POSITION *makeNewPosition(int row, int column);
int isSameField(FIELD field1, FIELD field2);
int isInRange(POSITION *position);

POSITION *makeNewPosition(int row, int column) {
    POSITION *newPosition = NULL;
    
    if((newPosition = (POSITION *)malloc(sizeof(POSITION))) == NULL){
        exit(1);
    }
    
    newPosition -> row = row;
    newPosition -> column = column;
    
    return newPosition;
}

int isSameField(FIELD field1, FIELD field2) {
    return field1 == field2;
}


int isInRange(POSITION *position) {
    if(position -> row < 0 || HEIGHT <= position -> row || position -> column < 0 || WIDTH <= position -> column ) {
        return 0;
    }
    
    return 1;
}

int isGoal(NODE *node) {
    return isSameField(node -> field, 0x0123456789abcdef);
}

void showField(FIELD field) {
    int i, j;
    FIELD mask = 0xf;
    
    for(i = HEIGHT - 1; i >= 0 ; i--) {
        for(j = WIDTH - 1; j >= 0; j--) {
            FIELD value = field >> 4 * (i * HEIGHT + j);
            value = value & mask;
            printf("%x ", value);
        }
        printf("\n");
    }
}

void showPath(NODE *node) {
    printf("/*path=======\n");
    while(node != NULL) {
        printf("-------\n");
        showField(node -> field);
        node = node -> parentNode;
    }
}

int shiftTo(int row, int column) {
    return 4 * (HEIGHT * WIDTH - (row * HEIGHT + column) - 1);
}

POSITION *getPositionOfSpace(FIELD field) {
    int i, j;
    FIELD bitMask = 0xf;
    for(i = HEIGHT - 1; i >= 0; i--) {
        for(j = WIDTH - 1; j >= 0; j--) {
            if(!(field & bitMask)) {
                return makeNewPosition(i, j);
            }
            field = field >> 4;
        }
    }
    
    return NULL;
}

