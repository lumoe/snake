#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>

#include "main.h"

#define DELAY_MILLI_SECONDS 100

#define RIGHT_BORDER 80
#define LOWER_BORDER 24

typedef struct snake_element{
    int x;
    int y;
    int direction;
    struct snake_element * next;
    struct snake_element * previous;
} snake_e;

typedef struct food {
    int x;
    int y;
} food_e;

snake_e * head = NULL;
snake_e * tail = NULL;
food_e* food = NULL;


int main(void){
    init_game();
    game_loop();
    return 0;
}

void init_game(){
    //Disable console echo
    system("stty -echo");
    head = malloc(sizeof(snake_e));
    head->x = 7;
    head->y = 7;
    head->direction = RIGHT;
    head->previous = NULL;

    tail = head;

    for(int i=6; i>3; i--){
        printf("loop lopp");
        grow_snake(i,7);
    }

    food = malloc(sizeof(food_e));
    food->x = 10;
    food->y = 10;

    srand(time(NULL));
}

void grow_snake(x,y){
    snake_e* new_element = malloc(sizeof(snake_e));
    new_element->x = x;
    new_element->y = y;
    tail->next = new_element;
    new_element->previous = tail;
    tail = new_element;
}

void draw_screen(){
    snake_e* current = tail;
    while(current != NULL) {
        gotoxy(current->x, current->y);
        putchar('x');
        current = current->previous;
    }

    gotoxy(food->x, food->y);
    putchar('o');
}

void recalc_snake(int direction){
    snake_e* cur = tail;

    while(cur->previous != NULL){
        cur->x = cur->previous->x;
        cur->y = cur->previous->y;
        cur = cur->previous;
    }
    switch(direction) {
        case UP:
            if(head->y -1 < 1) {
                head->y = 24;
            } else {
                head->y--;
            }

            break;
        case DOWN:
            if(head->y +1 > 24) {
                head->y=1;
            } else {
                head->y++;
            }
            break;
        case LEFT:
            if(head->x -1 < 1) {
                head->x = 80;
            } else {
                head->x--;
            }

            break;
        case RIGHT:
            if(head->x + 1 >80) {
                head->x = 1;
            } else {
                head->x++;
            }
            break;
    }
}

int get_snake_length(){
    int length = 1;
    snake_e* current = head;
    while(current != NULL) {
        gotoxy(current->x, current->y);
        putchar('x');
        current = current-> next;
        length++;
    }
    return length;
}

bool check_collision() {
    snake_e* cur = head->next;
    // Check collision with snake itself
    while(cur != NULL) {
        if((cur->x == head->x) && (cur->y == head->y)) {
            return true;
        }
        cur = cur->next;
    }
    return false;
}

void spawn_food() {
    food->x = ((double)rand() / RAND_MAX) * 80;
    food->y = ((double)rand() / RAND_MAX) * 24;
}

void increase_snake() {
    //Increase snake size by one element
    snake_e* new_element = malloc(sizeof(snake_e));

    if((tail->x - tail->previous->x) == 0) {
        new_element->x = tail->x;
    } else {
        new_element->x = tail->x - tail->previous->x;
    }

    if((tail->y - tail->previous->y) == 0) {
        new_element->x = tail->x;
    } else {
        new_element->y = tail->y - tail->previous->y;
    }

    tail->next = new_element;
    new_element->previous = tail;
    tail = new_element;
}

bool check_food(void) {
    if((head->x == food->x) && (head->y == food->y))
        return true;
    return false;
}

void game_loop() {
    char c;
    int current_direction = RIGHT;
    for(;;){
        fflush(stdin);
        if(kbhit()){
            c = mygetch();
        }

        switch(c) {
            case 'w': current_direction = UP;recalc_snake(UP); break;
            case 's': current_direction = DOWN; recalc_snake(DOWN); break;
            case 'a': current_direction = LEFT; recalc_snake(LEFT); break;
            case 'd': current_direction = RIGHT; recalc_snake(RIGHT); break;
        }

        if(check_collision()){
            exit(0);
        }

        if(check_food()) {
            increase_snake();
            spawn_food();
        }

        system("clear");
        draw_screen();
        debug();
        /*gotoxy(x,y);
        putchar('x'); */
        fflush(stdout);
        usleep(1000 * DELAY_MILLI_SECONDS);
    }
}

void debug(){
    gotoxy(0,0);
   /* snake_e* current = tail;
    while(current != NULL) {
        printf("%d,%d\n", current->x, current->y);
        current = current->previous;
    } */
    printf("food @ %d,%d\n",food->x,food->y);
    printf("head @ %d,%d\n",head->x,head->y);
}

void gotoxy(int x,int y) {
    printf("%c[%d;%df",0x1B,y,x);
}

char mygetch() {
    char c;
    system("stty raw");
    c= getchar();
    system("stty sane");
    //printf("%c",c);
    return(c);
}


int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
