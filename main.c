#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define W 20
#define H 20

typedef struct { int x, y; } P;

int main(void) {
    P s[400], food;
    int len = 3, dx = 1, dy = 0, score = 0, dead = 0;
    char map[H][W];
    srand((unsigned)time(NULL));
    for (int i = 0; i < len; i++) s[i] = (P){2 - i, 2};
    food = (P){rand() % W, rand() % H};
    while (!dead) {
        if (_kbhit()) {
            switch (_getch()) {
                case 'w': if (dy != 1) dx = 0, dy = -1; break;
                case 's': if (dy != -1) dx = 0, dy = 1; break;
                case 'a': if (dx != 1) dx = -1, dy = 0; break;
                case 'd': if (dx != -1) dx = 1, dy = 0; break;
                case 'q': dead = 1; break;
            }
        }
        for (int y = 0; y < H; y++) for (int x = 0; x < W; x++) map[y][x] = ' ';
        for (int i = len; i > 0; i--) s[i] = s[i - 1];
        s[0].x += dx; s[0].y += dy;
        if (s[0].x < 0 || s[0].x >= W || s[0].y < 0 || s[0].y >= H) dead = 1;
        for (int i = 1; i < len; i++) if (s[i].x == s[0].x && s[i].y == s[0].y) dead = 1;
        if (dead) break;
        if (s[0].x == food.x && s[0].y == food.y) {
            len++; score++;
            do {
                food.x = rand() % W;
                food.y = rand() % H;
                for (int i = 0; i < len && !(dead = 0); i++)
                    if (s[i].x == food.x && s[i].y == food.y) dead = 1;
            } while (dead);
            dead = 0;
        }
        map[food.y][food.x] = '*';
        for (int i = 0; i < len; i++) map[s[i].y][s[i].x] = i ? 'o' : '@';
        system("cls");
        printf("score: %d   wsad control, q quit\n", score);
        for (int y = 0; y < H + 2; y++) {
            for (int x = 0; x < W + 2; x++)
                putchar(y == 0 || y == H + 1 || x == 0 || x == W + 1 ? '#' : map[y - 1][x - 1]);
            putchar('\n');
        }
        Sleep(150);
    }
    printf("game over, score=%d\n", score);
    return 0;
}
