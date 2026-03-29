#include <stdio.h>
#include <string.h>
#include "playgame.h"

int main() {
    Point grid[5][6];
    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 6; j++) {
            grid[i][j].up = grid[i][j].down = grid[i][j].left = grid[i][j].right = 0;
        }
    }

    int row1, col1, row2, col2;
    int turn_count = 1;
    int score_A = 0, score_B = 0, squares_count = 0, t = 0;
    char scores[100];
    int i;
for (i = 0; i < 100; i++) {
    scores[i] = ' ';
}
    char currentPlayer = (turn_count % 2 == 0) ? 'B' : 'A';
    int x =0;
    while (squares_count < 20) {
        
        printf("\nPlayer %c, enter row1 col1 row2 col2: ", currentPlayer);
        
        if (scanf("%d %d %d %d", &row1, &col1, &row2, &col2) != 4) break;
        

        addLine(grid, row1, row2, col1, col2);
        int result = checkSquare(grid, row1, row2, col1, col2,&x);

        if (result > 0) {
            scores[t++] = currentPlayer;
            squares_count++;
            if (currentPlayer == 'A') score_A+=x; else score_B+=x;
            printf("Good turn player %c\n\n",currentPlayer);
            
        }
        else {
            if(currentPlayer == 'A') currentPlayer = 'B';
            else currentPlayer = 'A';
        }

        printGrid(grid, scores);
        printf("score of A = %d, score of B : %d",score_A,score_B);
        turn_count++;
    }

    printf("\nFinal Score - Player A: %d | Player B: %d\n", score_A, score_B);
    if(score_A==score_B) printf("DRAW");
    else
    printf("The winner is %c\n", score_A > score_B ? 'A' : 'B');
    return 0;
}
