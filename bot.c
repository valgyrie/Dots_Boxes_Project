#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bot.h"

void buildActions(Q_table *q, Point grid[5][6]) {
    q->sizeOfActions = 0;
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            if (grid[i][j].right == 0 && q->sizeOfActions < MAX_ACTIONS)
                q->actions[q->sizeOfActions++] = (Action){i, j, i, j + 1};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 6; j++)
            if (grid[i][j].down == 0 && q->sizeOfActions < MAX_ACTIONS)
                q->actions[q->sizeOfActions++] = (Action){i, j, i + 1, j};
}

void saveQ_table(Q_table *q) {
    FILE *f = fopen(Q_FILE, "w");
    if (!f) { perror("saveQ_table"); return; }
    fprintf(f, "STATES %d\n", q->sizeOfStates);
    for (int s = 0; s < q->sizeOfStates; s++) {
        fprintf(f, "STATE %d SCORE %d\n", s, q->states[s].score);
        fprintf(f, "GRID\n");
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 6; j++)
                fprintf(f, "%d %d %d %d\n",
                    q->states[s].points[i][j].up, q->states[s].points[i][j].down,
                    q->states[s].points[i][j].left, q->states[s].points[i][j].right);
        fprintf(f, "ACTIONS %d\n", q->sizeOfActions);
        for (int a = 0; a < q->sizeOfActions; a++)
            fprintf(f, "%d %d %d %d\n",
                q->actions[a].row1, q->actions[a].col1,
                q->actions[a].row2, q->actions[a].col2);
        fprintf(f, "QVALUES\n");
        for (int a = 0; a < q->sizeOfActions; a++)
            fprintf(f, "%.4f\n", q->table[s][a]);
        fprintf(f, "ENDSTATE\n");
    }
    fclose(f);
}

void loadQ_table(Q_table *q) {
    memset(q, 0, sizeof(Q_table));
    FILE *f = fopen(Q_FILE, "r");
    if (!f) return;
    int totalStates;
    fscanf(f, "STATES %d\n", &totalStates);
    for (int s = 0; s < totalStates && s < MAX_STATES; s++) {
        int idx, score;
        fscanf(f, "STATE %d SCORE %d\n", &idx, &score);
        q->states[s].score = score;
        fscanf(f, "GRID\n");
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 6; j++)
                fscanf(f, "%d %d %d %d\n",
                    &q->states[s].points[i][j].up, &q->states[s].points[i][j].down,
                    &q->states[s].points[i][j].left, &q->states[s].points[i][j].right);
        int numActions;
        fscanf(f, "ACTIONS %d\n", &numActions);
        q->sizeOfActions = numActions;
        for (int a = 0; a < numActions; a++)
            fscanf(f, "%d %d %d %d\n",
                &q->actions[a].row1, &q->actions[a].col1,
                &q->actions[a].row2, &q->actions[a].col2);
        fscanf(f, "QVALUES\n");
        for (int a = 0; a < numActions; a++)
            fscanf(f, "%f\n", &q->table[s][a]);
        fscanf(f, "ENDSTATE\n");
        q->sizeOfStates++;
    }
    fclose(f);
}

int botPlay(Q_table *q, Point grid[5][6], int *botScore, int *squaresCount, char scores[100], int *t) {
    int stateIndex = whichState(grid, *q, *botScore);
    if (stateIndex == -1) {
        State cur;
        memcpy(cur.points, grid, sizeof(cur.points));
        cur.score = *botScore;
        stateIndex = addState(q, cur);
    }
    buildActions(q, grid);
    if (q->sizeOfActions == 0) return 0;
    int actionIndex;                            
    
        actionIndex = selectAction(*q, stateIndex);
    Action a = q->actions[actionIndex];
    printf("\nBot plays: (%d,%d) -> (%d,%d)\n", a.row1, a.col1, a.row2, a.col2);
    addLine(grid, a.row1, a.row2, a.col1, a.col2);
    int x = 0;
    checkSquare(grid, a.row1, a.row2, a.col1, a.col2, &x);
    if (x > 0) {
        for (int j = 0; j < x; j++) scores[(*t)++] = 'B';
        *squaresCount += x;
        *botScore += x;
    }
    updateQ_value(grid, q, stateIndex, x);
    saveQ_table(q);
    return x > 0 ? 1 : 0;
}

int whichState(Point p[5][6], Q_table q, int score) {
    for (int k = 0; k < q.sizeOfStates; k++) {
        State s = q.states[k];
        int equalScores = (score == s.score) ? 1 : 0;
        int boolean = 1;
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 6; j++) {
                if (p[i][j].left != s.points[i][j].left) boolean = 0;
                if (p[i][j].right != s.points[i][j].right) boolean = 0;
                if (p[i][j].up != s.points[i][j].up) boolean = 0;
                if (p[i][j].down != s.points[i][j].down) boolean = 0;
            }
        if (boolean + equalScores == 2) return k;
    }
    return -1;
}

int selectAction(Q_table q, int state) {
    int index = state;
    if (index == -1) return 0;
    float bestAction = q.table[index][0];
    int whichAction = 0;
    for (int i = 1; i < q.sizeOfActions; i++) {
        if (q.table[index][i] > bestAction) {
            bestAction = q.table[index][i];
            whichAction = i;
        }
    }
    return whichAction;
}

int addState(Q_table *q, State s) {
    q->states[q->sizeOfStates] = s;
    for (int j = 0; j < 100; j++)
        q->table[q->sizeOfStates][j] = 0;
    q->sizeOfStates++;
    return q->sizeOfStates - 1;
}

void updateQ_value(Point p[5][6], Q_table *q, int state, int reward) {
    for (int j = 0; j < q->sizeOfActions; j++) {
        int s2 = tryAction(p, *q, q->actions[j], q->states[state].score);
        float max = 0;
        if (s2 != -1) {
            for (int k = 0; k < q->sizeOfActions; k++) {
                if (max < q->table[s2][k])
                    max = q->table[s2][k];
            }
        }
        q->table[state][j] += ALPHA * (reward + GAMMA * max - q->table[state][j]);
    }
}

int tryAction(Point points[5][6], Q_table q, Action a, int score) {
    Point temp[5][6];
    memcpy(temp, points, sizeof(temp));
    addLine(temp, a.row1, a.row2, a.col1, a.col2);
    return whichState(temp, q, score);
}