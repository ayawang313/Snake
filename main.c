#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define WIDTH 20
#define HEIGHT 20
#define MAX_SNAKE (WIDTH * HEIGHT)

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point body[MAX_SNAKE];
    Point food;
    int length;
    int dx;
    int dy;
    int score;
    int game_over;
    int win;
} Game;

static void clear_screen(void) {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    COORD home = {0, 0};
    DWORD written;
    DWORD size;

    if (console == INVALID_HANDLE_VALUE || !GetConsoleScreenBufferInfo(console, &info)) {
        putchar('\n');
        return;
    }

    size = (DWORD)info.dwSize.X * (DWORD)info.dwSize.Y;
    FillConsoleOutputCharacter(console, ' ', size, home, &written);
    FillConsoleOutputAttribute(console, info.wAttributes, size, home, &written);
    SetConsoleCursorPosition(console, home);
}

static int is_snake_at(const Game *game, int x, int y) {
    int i;
    for (i = 0; i < game->length; i++) {
        if (game->body[i].x == x && game->body[i].y == y) {
            return 1;
        }
    }
    return 0;
}

static int spawn_food(Game *game) {
    int empty[MAX_SNAKE];
    int count = 0;
    int x;
    int y;

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (!is_snake_at(game, x, y)) {
                empty[count++] = y * WIDTH + x;
            }
        }
    }

    if (count == 0) {
        game->win = 1;
        game->game_over = 1;
        return 0;
    }

    {
        int cell = empty[rand() % count];
        game->food.x = cell % WIDTH;
        game->food.y = cell / WIDTH;
    }
    return 1;
}

static void init_game(Game *game) {
    int i;

    game->length = 3;
    game->dx = 1;
    game->dy = 0;
    game->score = 0;
    game->game_over = 0;
    game->win = 0;
    for (i = 0; i < game->length; i++) {
        game->body[i].x = 2 - i;
        game->body[i].y = 2;
    }
    spawn_food(game);
}

static void handle_input(Game *game) {
    int key;

    if (!_kbhit()) {
        return;
    }

    key = _getch();
    if (key == 0 || key == 224) {
        key = _getch();
        if (key == 72 && game->dy != 1) {
            game->dx = 0;
            game->dy = -1;
        } else if (key == 80 && game->dy != -1) {
            game->dx = 0;
            game->dy = 1;
        } else if (key == 75 && game->dx != 1) {
            game->dx = -1;
            game->dy = 0;
        } else if (key == 77 && game->dx != -1) {
            game->dx = 1;
            game->dy = 0;
        }
        return;
    }

    if ((key == 'w' || key == 'W') && game->dy != 1) {
        game->dx = 0;
        game->dy = -1;
    } else if ((key == 's' || key == 'S') && game->dy != -1) {
        game->dx = 0;
        game->dy = 1;
    } else if ((key == 'a' || key == 'A') && game->dx != 1) {
        game->dx = -1;
        game->dy = 0;
    } else if ((key == 'd' || key == 'D') && game->dx != -1) {
        game->dx = 1;
        game->dy = 0;
    } else if (key == 'q' || key == 'Q') {
        game->game_over = 1;
    }
}

static void update_game(Game *game) {
    Point next = game->body[0];
    int i;
    int grow = 0;

    next.x += game->dx;
    next.y += game->dy;

    if (next.x < 0 || next.x >= WIDTH || next.y < 0 || next.y >= HEIGHT) {
        game->game_over = 1;
        return;
    }

    if (next.x == game->food.x && next.y == game->food.y) {
        grow = 1;
        if (game->length >= MAX_SNAKE) {
            game->win = 1;
            game->game_over = 1;
            return;
        }
    }

    for (i = 0; i < game->length - !grow; i++) {
        if (game->body[i].x == next.x && game->body[i].y == next.y) {
            game->game_over = 1;
            return;
        }
    }

    for (i = game->length; i > 0; i--) {
        game->body[i] = game->body[i - 1];
    }
    game->body[0] = next;

    if (grow) {
        game->length++;
        game->score++;
        spawn_food(game);
    }
}

static void render_game(const Game *game) {
    int x;
    int y;

    clear_screen();
    printf("score: %d   wsad/arrows control, q quit\n", game->score);
    for (y = 0; y < HEIGHT + 2; y++) {
        for (x = 0; x < WIDTH + 2; x++) {
            char cell = ' ';

            if (y == 0 || y == HEIGHT + 1 || x == 0 || x == WIDTH + 1) {
                cell = '#';
            } else if (game->food.x == x - 1 && game->food.y == y - 1) {
                cell = '*';
            } else {
                int i;
                for (i = 0; i < game->length; i++) {
                    if (game->body[i].x == x - 1 && game->body[i].y == y - 1) {
                        cell = i == 0 ? '@' : 'o';
                        break;
                    }
                }
            }
            putchar(cell);
        }
        putchar('\n');
    }
}

int main(void) {
    Game game;

    srand((unsigned)time(NULL));
    init_game(&game);

    while (!game.game_over) {
        handle_input(&game);
        update_game(&game);
        if (!game.game_over || game.win) {
            render_game(&game);
        }
        Sleep(150);
    }

    if (!game.win) {
        render_game(&game);
    }
    printf(game.win ? "you win, score=%d\n" : "game over, score=%d\n", game.score);
    return 0;
}
