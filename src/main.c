#include <stdint.h>
#include <pic32mx.h>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 128

#define DC_HIGH (PORTDSET = BIT(2))
#define DC_LOW (PORTDCLR = BIT(2))
#define RST_HIGH (PORTDSET = BIT(1))
#define RST_LOW (PORTDCLR = BIT(1))

#define BIT(x) 1 << x

// Ouputs:
// SCK  -> Pin 13	-> RG6
// MOSI -> Pin 11	-> RG8
// CS   -> Pin 10	-> RD4
// RST  -> Pin 6	-> RD1
// D/C  -> Pin 5	-> RD2

// Inputs:
// BTN1	-> Pin 4	-> RF1
// BTN2	-> Pin 34	-> RD5
// BTN3	-> Pin 36	-> RD6
// BTN4	-> Pin 37	-> RD7

typedef struct {
	float r, g, b;
} color;

typedef uint16_t spi_packet;

// In routines.S
void quicksleep(int delay);

// Gets states of buttons
int getButtonStates() {
    return (PORTD >> 5) & 0x7;
}

void spi_send(spi_packet data) {
	while(!(SPI2STAT & BIT(3))); // Wait until buffer is empty
	SPI2BUF = data;
}

// Initialize port settings
// Some parts of the function brought from mipslabmain.c
void ports_init() {
	// Input pins
	TRISFSET = BIT(1);	// Set button 1 as input
	TRISDSET = BIT(7) | BIT(6) | BIT(5); // Set button 2,3,4 as input

	// Set peripheral bus clock to same frequency as sysclock (80 MHz)
	SYSKEY = 0xAA996655;			// Unlock OSCCON, step 1
	SYSKEY = 0x556699AA;			// Unlock OSCCON, step 2
	while (OSCCON & BIT(21));		// Wait until PBDIV ready
	OSCCONCLR = BIT(20) | BIT(19);	// clear PBDIV bit <0,1>	(PBCLK = SYSCLK / 1)
	while (OSCCON & BIT(21));		// Wait until PBDIV ready
	SYSKEY = 0x0;					// Lock OSCCON

	// Set pins for display signals as output
	PORTG = BIT(8);		// Start with MOSI on high
	// PORTD = BIT(4);		// Start with CS on high
	TRISGCLR = BIT(8) | BIT(6);			// Set output for MOSI & clock
	TRISDCLR = BIT(4) | BIT(2) | BIT(1)	// Set output for CS, D/C & RST

	// SPI settings
	SPI2BRG = 4;			// Baud rate 
	SPI2STATCLR = BIT(6);	// SPIROV = 0	(no overflow has occurred)
	SPI2CON = 0;			// Clear SPI2CON (really necessary?)
	SPI2CONSET = BIT(10);	// MODE16 = 1 	(16-bit data width)
	SPI2CONSET = BIT(6);	// CKP = 1 		(idle on high level)
	SPI2CONSET = BIT(5); 	// MSTEN = 1 	(master mode)
	SPI2CONSET = BIT(15);	// ON = 1 		(switch on SPI module)
}

// Initialize settings on the display
void display_init() {
	
}

// Output pixel data to the display via SPI
void update_display(color display_buf[DISPLAY_HEIGHT][DISPLAY_WIDTH]) {

}

int main() {
	ports_init();
	display_init();

	color display_buf[DISPLAY_HEIGHT][DISPLAY_WIDTH] = {0};

	// Game loop
	while (1) {
		for (int y = 0; y < DISPLAY_HEIGHT; y++) {
			for (int x = 0; x < DISPLAY_WIDTH; x++) {
				color pixel_color = { 0, 0, 0 };
				
				if (x % 2 == 0) { // Test
					pixel_color.r = 1;
					pixel_color.g = 1;
					pixel_color.b = 1;
				}
				display_buf[y][x] = pixel_color;
			}
		}

		update_display(display_buf);
	}

    return 0;
}