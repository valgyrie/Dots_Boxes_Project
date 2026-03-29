#include <stdio.h>
#include <string.h>
#include "playgame.h"

void addLine(Point arr[5][6], int row, int row2, int col, int col2) {
    int differenceR = row2 - row;
    int differenceC = col2 - col;

    if (differenceR != 0 && differenceC != 0) {
        printf("Invalid turn\n");
        return;
    }

    if (differenceR < 0) { // Upward
        arr[row][col].up = 1;
        arr[row2][col2].down = 1;
    } else if (differenceR > 0) { // Downward
        arr[row][col].down = 1;
        arr[row2][col2].up = 1;
    }

    if (differenceC < 0) { // Leftward
        arr[row][col].left = 1;
        arr[row2][col2].right = 1;
    } else if (differenceC > 0) { // Rightward
        arr[row][col].right = 1;
        arr[row2][col2].left = 1;
    }
}

int checkSquare(Point arr[5][6], int row, int row2, int col, int col2) {
    int differenceC = col2 - col;
    int differenceR = row2 - row;
    int flag = 0;

    if (differenceR == 0) { // Horizontal line
        int c = (col < col2) ? col : col2;
        // Check square above
        if (row > 0 && arr[row][c].right == 1 && arr[row-1][c].right == 1 && 
            arr[row][c].up == 1 && arr[row][c+1].up == 1) flag = 1;
        // Check square below
        if (row < 4 && arr[row][c].right == 1 && arr[row+1][c].right == 1 && 
            arr[row][c].down == 1 && arr[row][c+1].down == 1) flag = 2;
    } else if(differenceC == 0) { // Vertical line
        int r = (row < row2) ? row : row2;
        // Check square left
        if (col > 0 && arr[r][col].down == 1 && arr[r][col-1].down == 1 && 
            arr[r][col].left == 1 && arr[r+1][col].left == 1) flag = 3;
        // Check square right
        if (col < 5 && arr[r][col].down == 1 && arr[r][col+1].down == 1 && 
            arr[r][col].right == 1 && arr[r+1][col].right == 1) flag = 4;
    }
    return flag;
}

void printGrid(Point arr[5][6], char a[100]) {
    printf("  0 1 2 3 4 5\n");
    int current_k = 0; 

    for (int i = 0; i < 5; i++) {
        printf("%d ", i);
        // Points and Horizontal lines
        for (int j = 0; j < 6; j++) {
            printf(".");
            if (j < 5 && arr[i][j].right == 1) printf("-");
            else if (j < 5) printf(" ");
        }
        printf("\n");

        // Vertical lines and Score Labels
        if (i < 4) {
            printf("  ");
            for (int j = 0; j < 6; j++) {
                if (arr[i][j].down == 1) printf("|");
                else printf(" ");
                
                // If this specific cell (i,j) is a completed square, print the letter
                if (j < 5) {
                    if (arr[i][j].right && arr[i+1][j].right && arr[i][j].down && arr[i][j+1].down) {
                        printf("%c", a[current_k++]);
                    } else {
                        printf(" ");
                    }
                }
            }
            printf("\n");
        }
    }
}
