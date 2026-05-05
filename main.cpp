#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "playgame.h"
#include "bot.h"

#define port 3040

pthread_mutex_t lock;
int my_turn = 1;

typedef struct player_info {
    int fd;
    Point (*grid)[6];
    int* score_A;
    int* score_B;
    int* squares_count;
    char* scores;
    int* t;
    int* turn_count;
} PlayerInfo;

// host thread
void* handle_remote_player(void* v) {
    PlayerInfo* p = (PlayerInfo*)v;
    int buff[4];
    int x = 0;

    while (*(p->squares_count) < 20) {

        while (my_turn == 1);

        if (*(p->squares_count) >= 20) break;

        int keep_turn = 1;
        while (keep_turn) {
            if (read(p->fd, buff, sizeof(buff)) <= 0) return NULL;

            pthread_mutex_lock(&lock);

            int y = addLine(p->grid, buff[0], buff[2], buff[1], buff[3]);
            while (y == 0) {
                printf("Remote player sent an invalid line, asking again\n");
                int flag = 2;
                write(p->fd, &flag, sizeof(int));
                pthread_mutex_unlock(&lock);
                if (read(p->fd, buff, sizeof(buff)) <= 0) return NULL;
                pthread_mutex_lock(&lock);
                y = addLine(p->grid, buff[0], buff[2], buff[1], buff[3]);
            }

            int result = checkSquare(p->grid, buff[0], buff[2], buff[1], buff[3], &x);

            if (result > 0) {
                for (int j = 0; j < x; j++) p->scores[(*p->t)++] = 'B';
                *p->score_B += x;
                *p->squares_count += x;
                printf("Good turn player B\n\n");
            } else {
                *(p->turn_count) += 1;
                my_turn = 1;
                keep_turn = 0;
            }

            printGrid(p->grid, p->scores);
            printf("score of A = %d, score of B : %d\n", *(p->score_A), *(p->score_B));

            write(p->fd, p->grid, sizeof(Point) * 5 * 6);
            write(p->fd, p->scores, 100);
            write(p->fd, p->score_A, sizeof(int));
            write(p->fd, p->score_B, sizeof(int));
            int flag = (result > 0) ? 1 : 0;
            write(p->fd, &flag, sizeof(int));

            pthread_mutex_unlock(&lock);

            if (*(p->squares_count) >= 20) return NULL;
        }
    }
    return NULL;
}

int main() {
    Point grid[5][6];
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 6; j++)
            grid[i][j].up = grid[i][j].down = grid[i][j].left = grid[i][j].right = 0;

    int row1, col1, row2, col2;
    int turn_count = 1;
    int score_A = 0, score_B = 0, squares_count = 0, t = 0;
    char scores[100];
    for (int i = 0; i < 100; i++) scores[i] = ' ';

    printf("Play against a bot (b), local human (l), or remote human (h) : ");
    char a;
    scanf(" %c", &a);
    int bot = (a == 'b') ? 1 : 0;

    Q_table q;
    loadQ_table(&q);
    pthread_mutex_init(&lock, NULL);
    int client_fd = -1;
    char b = ' ';
    int is_host = 0;

    if (a != 'b' && a != 'l') {
        printf("Do you want to be the host? Y/N: ");
        scanf(" %c", &b);

        // host setup
        if (b == 'Y' || b == 'y') {
            is_host = 1;

            struct sockaddr_in add;
            add.sin_family = AF_INET;
            add.sin_addr.s_addr = INADDR_ANY;
            add.sin_port = htons(port);

            int server_fd = socket(AF_INET, SOCK_STREAM, 0);

            if (bind(server_fd, (struct sockaddr*)&add, sizeof(add)) < 0) {
                perror("bind failed");
                close(server_fd);
            }

            if (listen(server_fd, 1) < 0) {
                perror("listen failed");
                close(server_fd);
            }

            printf("Waiting for remote player...\n");
            client_fd = accept(server_fd, (struct sockaddr*)&add, NULL);
            printf("Remote player connected!\n");

            PlayerInfo* p_info = (PlayerInfo*)malloc(sizeof(PlayerInfo));
            p_info->fd            = client_fd;
            p_info->grid          = grid;
            p_info->score_A       = &score_A;
            p_info->score_B       = &score_B;
            p_info->squares_count = &squares_count;
            p_info->scores        = scores;
            p_info->t             = &t;
            p_info->turn_count    = &turn_count;

            pthread_t tid;
            pthread_create(&tid, NULL, handle_remote_player, p_info);
            pthread_detach(tid);

        } else {
            // client code
            client_fd = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in serv_addr;
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port   = htons(port);

            char host_ip[20];
            printf("Enter Host IP: ");
            scanf("%s", host_ip);
            serv_addr.sin_addr.s_addr = inet_addr(host_ip);
            connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
            printf("Connected to host!\n");

            Point grid_r[5][6];
            char scores_r[100];
            int sa, sb, flag;

            while (1) {
                read(client_fd, grid_r, sizeof(Point) * 5 * 6);
                read(client_fd, scores_r, 100);
                read(client_fd, &sa, sizeof(int));
                read(client_fd, &sb, sizeof(int));
                read(client_fd, &flag, sizeof(int));
                printGrid(grid_r, scores_r);
                printf("score of A = %d, score of B : %d\n", sa, sb);

                if (flag == 1) continue;

                do {
                    printf("\nPlayer B, enter row1 col1 row2 col2: ");
                    if (scanf("%d %d %d %d", &row1, &col1, &row2, &col2) != 4) goto done;
                    int buff[4] = {row1, col1, row2, col2};
                    write(client_fd, buff, sizeof(buff));

                    read(client_fd, grid_r, sizeof(Point) * 5 * 6);
                    read(client_fd, scores_r, 100);
                    read(client_fd, &sa, sizeof(int));
                    read(client_fd, &sb, sizeof(int));
                    read(client_fd, &flag, sizeof(int));
                    printGrid(grid_r, scores_r);
                    printf("score of A = %d, score of B : %d\n", sa, sb);
                    if (flag == 2) printf("The line is drawn before\n");
                    else if (flag == 1) printf("Good turn player B\n\n");

                } while (flag == 1 || flag == 2);
            }
            done:
            close(client_fd);
            return 0;
        }
    }

    // host / bot / local game loop
    while (squares_count < 20) {
        char currentPlayer = (turn_count % 2 == 0) ? 'B' : 'A';
        int x = 0;

        if (is_host && currentPlayer == 'B') {
            my_turn = 0;
            while (my_turn == 0);
            continue;
        }

        printf("\nPlayer %c, enter row1 col1 row2 col2: ", currentPlayer);

        // bot
        if (bot == 1 && currentPlayer == 'B') {
            if (botPlay(&q, grid, &score_B, &squares_count, scores, &t) == 0) {
                turn_count++;
            }
            printGrid(grid, scores);
            printf("score of A = %d, score of B : %d\n", score_A, score_B);
            continue;
        }

        if (scanf("%d %d %d %d", &row1, &col1, &row2, &col2) != 4) break;

        int y = addLine(grid, row1, row2, col1, col2);
        if (y == 0) {
            printf("The line is drawn before\n");
            continue;
        }

        int result = checkSquare(grid, row1, row2, col1, col2, &x);

        if (result > 0) {
            for (int j = 0; j < x; j++)
                scores[t++] = currentPlayer;
            squares_count += x;
            if (currentPlayer == 'A') score_A += x; else score_B += x;
            printf("Good turn player %c\n\n", currentPlayer);
        } else {
            turn_count++;
        }

        printGrid(grid, scores);
        printf("score of A = %d, score of B : %d\n", score_A, score_B);

        if (is_host) {
            write(client_fd, grid, sizeof(Point) * 5 * 6);
            write(client_fd, scores, 100);
            write(client_fd, &score_A, sizeof(int));
            write(client_fd, &score_B, sizeof(int));
            int flag = (result > 0) ? 1 : 0;
            write(client_fd, &flag, sizeof(int));
        }
    }

    printf("\nFinal Score - Player A: %d | Player B: %d\n", score_A, score_B);
    if (score_A == score_B) printf("DRAW\n");
    else printf("The winner is %c\n", score_A > score_B ? 'A' : 'B');

    if (client_fd != -1) close(client_fd);
    pthread_mutex_destroy(&lock);
    return 0;
}
