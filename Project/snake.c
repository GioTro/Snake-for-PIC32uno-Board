/**
* This is a basic snake game written in C for UNO32
* This file handles most of the game logic.
* Written by Giorgos Trogadas and Caner Ulug
* 02-12-2018
*/

#include <stdlib.h>
#include <pic32mx.h>
#include "header.h"

int grow = 0;
int r=4;

/* Generates a "random" number */
int randomNr(int j) {
		int i;
		for (i = 0; i < 512; i++) {
			r = r + dataArray[i];
		}
    r = (r*377 + 3) % 137;
    return r%j;
}

/**
*	Changes direction through a buffer
*   The actual direction is changed when the
*   new position of the head is calculated.
*/
void changeDirection(int newDirection) {
	if (direction == newDirection) {
		return;
	}
	if ((newDirection == down & direction != up) | (newDirection == up & direction != down)) {
		directionBuffer = newDirection;
	}
	if ((newDirection == left & direction != right) | (newDirection == right & direction != left)) {
		directionBuffer = newDirection;
	}
}

/* Check game over condition */
void checkGameOver() {
	int i;
	for (i = 1; i < snakeLength; i++) {
		if (snake[i].valid) {
			if (snake[i].x == snake[0].x & snake[i].y == snake[0].y) {
				gameOverCond = 1;
				break;
			}
			if (snake[i].x > 125 | snake[i].x < 2) {
				gameOverCond = 1;
				break;
			}
			if (snake[i].y > 29 | snake[i].y < 2) {
				gameOverCond = 1;
				break;
			}
		}
	}
}

/***
* Redraws the snake.
* The snake is stored in an array.
* Each body part is moved to the right in the array and
* a new "head" is added to the [0] position
* with coordinates dependent on the current direction.
*/

void updateSnake() {
	int i;
	for (i = snakeLength-1; i >= 0; i--) {
		snake[i+1] = snake[i];
	}
	snake[0] = snake[1];

	// IF not growing remove the tail otherwise keep the tail.
	if (!grow) {
		for (i = 0; i < snakeLength; i++) {
			if (!snake[i].valid) {
				snake[i-1].valid = 0;
				break;
			}
		}
	}
	grow = 0;

	direction = directionBuffer; // Change direction
	// Calculate the new position of the head.
	if (direction == up) {
		snake[0].y = snake[0].y - 1;
	} else if (direction == down) {
		snake[0].y = snake[0].y + 1;
	} else if (direction == left) {
		snake[0].x = snake[0].x - 1;
	} else if (direction == right) {
		snake[0].x = snake[0].x + 1;
	}

	if(food[0].valid) {
		if(food[0].x==snake[0].x && food[0].y==snake[0].y ) {
			grow =1;
			food[0].valid=0;
      PORTE++;
      check=1;
		}
	}

	if(!(food[0].valid)) {
		initFood();
	}
	checkGameOver();
}

/**
* Initializes the game to starting state.
*/

void initSnake() {
	int i;
  PORTE=0;
  speed = 8;
  initFood();
	for (i = 0; i < snakeLength; i++) {
		snake[i].x = 0;
		snake[i].y = 0;
		snake[i].valid = 0;
	}

	for (i = 0; i < 10; i++) {
		snake[i].x = 50+i;
		snake[i].y = 16;
		snake[i].valid = 1;
	}
	gameOverCond = 0;
	grow = 0;
  	check = 0;
}

/* Initializes a new food on the board. */
void initFood() {
	food[0].x=randomNr(123)+3;
	food[0].y=randomNr(27)+3;
	food[0].valid=1;
}

/* Redraws the snake on the game board */
void snakeDraw() {
	int i;
	for (i = 0; i < snakeLength; i++) {
		if (snake[i].valid) {
			int x = snake[i].x;
			int y = snake[i].y;
			displayPixel(x, y);
		}
	}
}

	/**
	* This utilize a for loop instead of the frameArray
	* using frameArray introduces side effects which we haven't debugged.
	*/
  void frameDraw() {
    int i;
    for (i = 0; i < 128; i++) {
      displayPixel(i, 0);
      displayPixel(i, 31);
    }
    for (i = 0; i < 32; i++) {
      displayPixel(0, i);
      displayPixel(127, i);
    }
}

/* Draws the food onto the game board */
void foodDraw() {
		if (food[0].valid) {
			int x = food[0].x;
			int y = food[0].y;
			displayPixel(x, y);
		}
}
