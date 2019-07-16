#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

void game_loop();
void init_game();
void draw_screen();
void gotoxy(int, int);
int khbit();
char mygetch();


enum {
    UP, DOWN, LEFT, RIGHT
} direction;

#endif // MAIN_H_INCLUDED
