/**
* Header file used for a basic snake game written for the UNO32 ChipKit board.
* Written by Giorgos Trogadas and Caner Ulug.
*/

#include <stdlib.h>
#include <stdint.h>
//#include <pic32mx.h>

#define snakeLength 50
#define up 10
#define down 11
#define left 12
#define right 13

typedef struct {
    int x;
    int y;
    int valid;
} body;

//Global variables
body snake[50];
body food[1];
int gameOverCond;
int grow;
int direction;
int directionBuffer;
uint8_t dataArray[512];

int r;
int pause;
int check;
int speed;


/* Functioner from display.c */
void sleep(int);
void sendSPI(uint8_t);
void displayPixel(int, int);
void display_update(void);
void frame_update(void);
void clearDisplay();
void display_init();
void frameDraw();

/* Function from snake.c */
void changeDirection(int );
void updateSnake();
void initSnake();
void snakeDraw();
void initFood();
void foodDraw();
