#include <stdint.h>
#include <pic32mx.h>

#include "rendering.h"
#include "controls.h"

#define BIT(x) 1 << x

#define DISPLAY_DATA_MODE (PORTFSET = BIT(4))
#define DISPLAY_COMMAND_MODE (PORTFCLR = BIT(4))
#define RST_LOW (PORTDCLR = BIT(1))
#define RST_HIGH (PORTDSET = BIT(1))
#define CS_LOW (PORTDCLR = BIT(4))
#define CS_HIGH (PORTDSET = BIT(4))

// Ouputs:
// Display pin -> board pin -> chip signal
// SCK  -> Pin 13	-> RG6
// MOSI -> Pin 11	-> RG8
// CS   -> Pin 10	-> RD4
// RST  -> Pin x	-> RD1
// D/C  -> Pin 39	-> RF4

// Inputs:
// BTN1	-> Pin 4	-> RF1
// BTN2	-> Pin 34	-> RD5
// BTN3	-> Pin 36	-> RD6
// BTN4	-> Pin 37	-> RD7

// Mostly for debugging
void setLights(uint8_t value) {
	volatile uint8_t *porte = (uint8_t*)0xbf886110;
	*porte = value;
} 

// In routines.S
void quicksleep(int delay);


uint8_t spi_send(uint8_t data) {
	while(!(SPI2STAT & BIT(3))); // Wait until transmit buffer is empty
	SPI2BUF = data;
	while(!(SPI2STAT & BIT(0))); // Wait until receive buffer is empty
	return SPI2BUF;
}

// Initialize port settings
// Some parts of the function brought from mipslabmain.c
void ports_init() {
	// Input pins
	TRISFSET = BIT(1);	// Set button 1 as input
	TRISDSET = BIT(7) | BIT(6) | BIT(5); // Set button 2,3,4 as input

	TRISECLR = 0xFF; // Set lights as output

	// Set peripheral bus clock to same frequency as sysclock (80 MHz)
	SYSKEY = 0xAA996655;			// Unlock OSCCON, step 1
	SYSKEY = 0x556699AA;			// Unlock OSCCON, step 2
	while (OSCCON & BIT(21));		// Wait until PBDIV ready
	OSCCONSET = BIT(20) | BIT(19);	// clear PBDIV bit <0,1>	(PBCLK = SYSCLK / 1)
	while (OSCCON & BIT(21));		// Wait until PBDIV ready
	SYSKEY = 0x0;					// Lock OSCCON

	// Set pins for display signals as output
	AD1PCFG = 0xFFFF;
	PORTGSET = BIT(8);		// Start with MOSI on high
	PORTDSET = BIT(4);		// Start with CS on high
	RST_HIGH;				// Reset is active low
	TRISGCLR = BIT(8) | BIT(6); // Set output for MOSI & clock
	TRISDCLR = BIT(4); // Set output for CS
	TRISFCLR = BIT(4); // Set output for D/C
	ODCF = 0x0;
	ODCG = 0x0;
	ODCD = 0x0;

	// SPI settings
	SPI2BRG = 4;			// Baud rate
	SPI2STATCLR = BIT(6);	// SPIROV = 0	(no overflow has occurred)
	SPI2CON = 0;			// Clear SPI2CON (really necessary?)
	// SPI2CONSET = BIT(10);	// MODE16 = 1 	(16-bit data width)
	SPI2CONSET = BIT(6);	// CKP = 1 		(idle on high level)
	SPI2CONSET = BIT(5); 	// MSTEN = 1 	(master mode)
	SPI2CONSET = BIT(15);	// ON = 1 		(switch on SPI module)
}

// Initialize settings on the display
void display_init() {
	CS_LOW;
	
	DISPLAY_COMMAND_MODE;
	spi_send(0x01); // Software reset
	quicksleep(2000000);
	spi_send(0x11); // Turn off sleep mode
	quicksleep(2000000);

	spi_send(0xb1);
	DISPLAY_DATA_MODE;
	spi_send(0x01);
	spi_send(0x2c);
	spi_send(0x2d);

	// X domain of drawing area (CASET)
	spi_send(0x2A);
	DISPLAY_DATA_MODE;
	spi_send(0);   // X start
	spi_send(0);   // X start
	spi_send(0);   // X end
	spi_send(127); // X end

	// Y domain of drawing area (RASET)
	DISPLAY_COMMAND_MODE;
	spi_send(0x2B);
	DISPLAY_DATA_MODE;
	spi_send(0);   // Y start
	spi_send(0);   // Y start
	spi_send(0);   // Y end
	spi_send(127); // Y end

	// Interface pixel format (COLMOD)
	DISPLAY_COMMAND_MODE;
	spi_send(0x3A);
	DISPLAY_DATA_MODE;
	spi_send(0x5); // 16-bit pixels

	DISPLAY_COMMAND_MODE;
	spi_send(0x29); // Display on mode

	CS_HIGH;
}

// Output pixel data of a column to the display via SPI
void update_display(color column_buf[DISPLAY_HEIGHT], int column) {
	CS_LOW;	// Enable transmission
	DISPLAY_COMMAND_MODE;
	spi_send(0x2C); // Memory write command
	DISPLAY_DATA_MODE;

	// SPI buffer is 32-bit and thus holds two 16-bit pixels, which might increase performance
	// In the meantime send one pixel at a time
	for (int y = 0; y < DISPLAY_HEIGHT; y++) {
		color pixel = column_buf[y];
		// concat(R<0:4>, G<0:5>, B<0:4>)
		int pixel_packet = (pixel.r & 0x1F << 11) | (pixel.g & 0x3F << 5) | (pixel.b & 0x1F);

		spi_send((uint8_t)(pixel_packet >> 8));
		spi_send((uint8_t)(pixel_packet & 0xFF));
	}
	CS_HIGH; // Disable transmission
}

int main() {
	ports_init();
	display_init();
				CS_LOW;	// Enable transmission
				DISPLAY_COMMAND_MODE;
				spi_send(0x2C); // Memory write command
				DISPLAY_DATA_MODE;

	player p = { 0.0f, 0.0f, 0.0f };
	
	// Game loop
	while (1) {
		setLights(0x80);
		DISPLAY_COMMAND_MODE;
		CS_HIGH;
		PORTGSET = BIT(6) | BIT(8);
		quicksleep(5000000);
		CS_LOW;
		DISPLAY_DATA_MODE;
		setLights(0);
		PORTGCLR = BIT(6) | BIT(8);
		quicksleep(5000000);

		color column_buf[DISPLAY_HEIGHT];

		// Test
		for (int y = 0; y < DISPLAY_HEIGHT; y++) {
			for (int x = 0; x < DISPLAY_WIDTH; x++) {
				color pixel_color = { 0, 0, 0 };
				
				if (x % 2 == 0) {
					pixel_color.r = 0xFF;
					pixel_color.g = 0xFF;
					pixel_color.b = 0xFF;
				}
				// spi_send(0);
				// update_display(column_buf);
			}
		}

		float sinAngle = sin(p.facingAngle);
		float cosAngle = cos(p.facingAngle);

		// Execute raycasting logic and draw for each column on the screen
		// for (int x = 0; x < DISPLAY_WIDTH; x++) {
		// 	render_column(column_buf, x, p, sinAngle, cosAngle);
		// 	update_display(column_buf, x);
		// }

		move_player(p, sinAngle, cosAngle);
	}

    return 0;
}