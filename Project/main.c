/**
* Main file for a snake game for the uno32 board
* Using the basic I/O shield.
* Written by Giorgos Trogadas and Caner Ulug
* 02-12-2018
*/


#include <stdint.h>
#include <stdlib.h>
#include <pic32mx.h>
#include "header.h"

int counter;
int pause=0;
int speed=8;

/**
* Configuration file from Lab3.
*/
void labinit( void )
{

	TRISD |= 0x000007f0; // change button to input
	TRISF |= 0x2;
	T2CON = 0x70; //Control register. Prescaling to 1:256.
	PR2 = (80000000/256)/50; //Set the period register accordingly. Divide by ten because we count 10 overflows before tick is called.
	TMR2 = 0; //Initialize the clock at 0.
	T2CONSET = 0x8000; //Starts the timer.
	PORTE = 0; //Initialize the light/"score"

	IFS(0) &= ~0x180;
	IPC(2) |= 0x1f; // Interupt priority register. Bit 0-1 subpriority 2-4 is priority. This sets all to max.
	IPC(1) |= 0x11000000; // Interrupt priority for the switch.
	IEC(0) |= 0x180; //Enable interupt for timer and switchbutton. Bit9 timer2, bit8 for the switch button. (interupt enable control)
	enable_interrupt(); // Calls the ei assembly instruction.
	return;
}

/**
* User defined interupt Service routine.
* We used a similar setup to the one in lab3
* This service routine is called from Vector.S
*/

void user_isr( void )
{
	counter++;
	// If switch then we pause
	if (IFS(0)&0x80) {
		if(!pause) {
		pause=1;
	}
	else pause=0;
	}

	// Changes the speed depending on the score
	if(PORTE%4==0 && speed>1 && check) {
		speed--;
		check=0;
	}

	/**
	* This service routine is called every 20ms by timer 2
	* this frequency determines how often the snake moves.
	*/
	if (counter % speed == 0 && !pause) {
		sleep(100);
		updateSnake();
		sleep(100);
	}
	IFS(0) = 0;
}

/*
* Function that polls the 4 buttons that determine the direction.
*/
	int getbtns(void) {
	  int out = (PORTD >> 5) & 0x7;
	  int f = ((PORTF >> 1) & 1) << 3; // Portf is the 4th button
	  return out|f;
	}

/*
* Take an action based on the state of the buttons.
*/
void readButtons() {
	if (((getbtns()>>3)&1) == 1) {
		changeDirection(down);
	}
	if (((getbtns()>>2)&1) == 1) {
		changeDirection(left);
  	}
  	if (((getbtns()>>1)&1) == 1) {
  		changeDirection(right);
  	}
 	 if ((getbtns()&1) == 1) {
 	 	changeDirection(up);
  	}
}

/**
* Blinking routine when game is over.
*/

void towelBlinkenLights( void ) {
	int i;
	int hold = PORTE;

	PORTE = 1;
	for (i = 0; i < 3; i++) {
		while (PORTE < 128) {
			pauseAwareSleep(100000);
			PORTE = PORTE << 1;
		}
		while ( PORTE > 1) {
			pauseAwareSleep(100000);
			PORTE = PORTE >> 1;
		}
	}
	if (!pause) {
		return;
	}
	//00011000
	//00111100
	//01111110
	//11111111
	for (i = 0; i < 3; i++) {
		PORTE = 0;
		pauseAwareSleep(400000);
		PORTE = 0x18;
		pauseAwareSleep(400000);
		PORTE = 0x3c;
		pauseAwareSleep(400000);
		PORTE = 0x7e;
		pauseAwareSleep(400000);
		PORTE = 255;
		if (i == 1) {
			break;
		}
		pauseAwareSleep(400000);
		PORTE = 0x7e;
		pauseAwareSleep(400000);
		PORTE = 0x3c;
		pauseAwareSleep(400000);
		PORTE = 0x18;
		pauseAwareSleep(400000);
	}
	if (!pause) {
		return;
	}
	PORTE = 255;
	for (i = 0; i < 3; i++) {
		pauseAwareSleep(1000000);
		PORTE = 0;
		pauseAwareSleep(1000000);
		PORTE = 255;
	}
	pauseAwareSleep(1000000);
	while (pause) {
		PORTE = hold;
		pauseAwareSleep(3000000);
		if (!pause) {
			break;
		}
		PORTE = 0;
		pauseAwareSleep(2000000);
	}
}

void pauseAwareSleep( int cyc ) {
	int i;
	for (i = cyc; i > 0; i--) {
		if (!pause) {
			break;
		}
	}
}

int main(void) {
  /*********! CONFIG FROM LAB
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */

	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;

	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;

	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);

	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
    SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;

	display_init();
	labinit();
	initSnake();

	changeDirection(left);

	IFS(0) = 0;

	// Mainloop
	while(1) {
		clearDisplay();
		snakeDraw();
		foodDraw();
		frameDraw();
		display_update();

		if(!gameOverCond&&!pause) {
		  	//frame_update();
			readButtons();
		}

		if (gameOverCond) {
			speed = 8;
			int hold = PORTE;
			frameDraw();
			pause = 1;
			towelBlinkenLights();
			clearDisplay();
			initSnake();
			direction = left;
			directionBuffer = left;
		}
	}
	return 0;
}
