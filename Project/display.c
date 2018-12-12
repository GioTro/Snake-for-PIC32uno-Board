
#include <stdint.h>
#include <pic32mx.h>
#include "header.h"
#include "frameArray.h"

//. The D/C pin is set high for display buffer access and low for
//command access (from documentation).
#define SWITCH_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define SWITCH_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_RESET (PORTGCLR = 0x200)
#define DISPLAY_DONT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_ (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_ (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

#define DATA_ARRAY_SIZE 512



/* Simple delay */
void sleep(int cycles) {
	int i;
	for(i = cycles; i > 0; i--);
}

/* spi_send is taken from Lab3 */
void spi_send(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
}

/* * * *
* Places a pixel in a dataArray that correpsonds to the
* pixel with coordinates x and y.
*/

void displayPixel(int x, int y) {
	if(x<128 && y<32 && !(x < 0) && !(y < 0)) { // check If coordinates are valid
		int offset = y % 8; //   0 is upper pixel, 8 is lowest pixel
		int page = y / 8;
		int pos = page*128 + x;
		dataArray[pos] = dataArray[pos] | (0x1 << offset);
	}
}

/* Sends data to screen */
void display_update(void) {
	int i;
	for(i = 0; i < DATA_ARRAY_SIZE; i++) {
		spi_send(dataArray[i]);
	}
}

/* Clears data in the dataArray */
void clearDisplay(void) {
	int i;
	for(i = 0; i < DATA_ARRAY_SIZE; i++) {
		dataArray[i] = 0x0;
	}
}

/**
* Screen configuration.
*/
void display_init(void) {
	SWITCH_TO_COMMAND_MODE;
	sleep(10);
	DISPLAY_ACTIVATE_;
	sleep(1000000);

	spi_send(0xAE);
	DISPLAY_RESET;
	sleep(10);
	DISPLAY_DONT_RESET;
	sleep(10);

	spi_send(0x8D);
	spi_send(0x14);

	spi_send(0xD9);
	spi_send(0xF1);

	DISPLAY_ACTIVATE_VBAT;
	sleep(10000000);

	spi_send(0xA1);
	spi_send(0xC8);

	spi_send(0xDA);
	spi_send(0x20);

	spi_send(0x20);
	spi_send(0x0);

	spi_send(0xAF);
	sleep(100);
	SWITCH_TO_DATA_MODE;
}
