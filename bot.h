#ifndef BOT_H
#define BOT_H

#include "playgame.h"

#define GAMMA       0.8f
#define ALPHA       0.1f
#define EPSILON     0.2f
#define MAX_STATES  100
#define MAX_ACTIONS 100
#define Q_FILE      "qtable.txt"

typedef struct {
    Point points[5][6];
    int   score;
} State;

typedef struct {
    int row1, col1, row2, col2;
} Action;

typedef struct {
    float  table[MAX_STATES][MAX_ACTIONS];
    State  states[MAX_STATES];
    Action actions[MAX_ACTIONS];
    int    sizeOfStates;
    int    sizeOfActions;
} Q_table;

int  whichState(Point p[5][6], Q_table q, int score);
int  selectAction(Q_table q, int state);
int  addState(Q_table *q, State s);
int  tryAction(Point points[5][6], Q_table q, Action a, int score);
void buildActions(Q_table *q, Point grid[5][6]);
void updateQ_value(Point p[5][6], Q_table *q, int state, int reward);
void saveQ_table(Q_table *q);
void loadQ_table(Q_table *q);
int  botPlay(Q_table *q, Point grid[5][6], int *botScore, int *squaresCount, char scores[100], int *t);

#endif