#include <stdint.h>
#include <pic32mx.h>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 128

#define BIT(x) 1 << x

// Connections:
// SCK  -> Port 13	-> 
// MOSI -> Port 11	-> SDO
// CS   -> Port 10	-> SS2
// RST  -> Port 6	-> 
// D/C  -> Port 5	-> 

typedef struct {
	float r, g, b;
} color;

typedef uint8_t spi_packet;

void spi_send(spi_packet data) {
	while(!(SPI2STAT & 0x8)); // Wait until buffer is empty
	SPI2BUF = data;
}

// Initialize port settings
// Some parts of the function brought from mipslabmain.c
void ports_init() {
	// Set peripheral bus clock to same frequency as sysclock (80 MHz)
	SYSKEY = 0xAA996655;		// Unlock OSCCON, step 1
	SYSKEY = 0x556699AA;		// Unlock OSCCON, step 2
	while (OSCCON & (1 << 21));	// Wait until PBDIV ready
	OSCCONCLR = 0x180000;		// clear PBDIV bit <0,1>	(PBCLK = SYSCLK / 1)
	while (OSCCON & (1 << 21));	// Wait until PBDIV ready
	SYSKEY = 0x0;				// Lock OSCCON

	// Set pins for display signals as output
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;

	// SPI settings
	SPI2BRG = 4;			// Baud rate 
	SPI2STATCLR = 0x40;		// SPIROV = 0	(no overflow has occurred)
	SPI2CON = 0;			// Clear SPI2CON (really necessary?)
	// SPI2CONSET = 0x400;		// MODE16 = 1 	(if 16-bit data width)
	SPI2CONSET = 0x40;		// CKP = 1 		(idle on high level)
	SPI2CONSET = 0x20; 		// MSTEN = 1 	(master mode)
	SPI2CONSET = 0x8000;	// ON = 1 		(switch on SPI module)
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