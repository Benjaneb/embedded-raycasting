#include <stdint.h>
#include <math.h>
#include <pic32mx.h>

#include "rendering.h"
#include "controls.h"

#define BIT(x) 1 << x

#define DISPLAY_DATA_MODE (PORTFSET = BIT(4))
#define DISPLAY_COMMAND_MODE (PORTFCLR = BIT(4))
#define RST_LOW (PORTBCLR = BIT(12))
#define RST_HIGH (PORTBSET = BIT(12))
#define CS_LOW (PORTBCLR = BIT(10))
#define CS_HIGH (PORTBSET = BIT(10))

// Software SPI
#define SCK_LOW (PORTBCLR = BIT(4))
#define SCK_HIGH (PORTBSET = BIT(4))
#define MOSI_LOW (PORTBCLR = BIT(8))
#define MOSI_HIGH (PORTBSET = BIT(8))


#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

// Ouputs:
// Display pin -> board pin -> chip signal

// Hardware SPI (not working)
// SCK  -> Pin 13	-> RG6
// MOSI -> Pin 11	-> RG8
// CS   -> Pin 10	-> RD4

// Software SPI
// SCK	-> Pin A1	-> RB4
// MOSI	-> Pin A2	-> RB8
// CS	-> Pin A3	-> RB10

// RST  -> Pin A4	-> RB12
// D/C  -> Pin 39	-> RF4

// Inputs:
// BTN1	-> Pin 4	-> RF1
// BTN2	-> Pin 34	-> RD5
// BTN3	-> Pin 36	-> RD6
// BTN4	-> Pin 37	-> RD7

// In routines.S
void quicksleep(int delay);

// Mostly for debugging
void setLights(uint8_t value) {
	volatile uint8_t *porte = (uint8_t*)0xbf886110;
	*porte = value;
} 

// Software SPI via bit banging
void sw_spi_send(uint8_t data) {
	CS_LOW;
	for (int i = 0; i < 8; i++) {
		if (data & BIT(i))
			MOSI_HIGH;
		else
		 	MOSI_LOW;
		SCK_HIGH;
		quicksleep(20);
		SCK_LOW;
		quicksleep(20);
	}
	CS_HIGH;
}

void spi_send(uint8_t data) {
	sw_spi_send(data);
	// while(!(SPI2STAT & BIT(3))); // Wait until transmit buffer is empty
	// SPI2BUF = data;
}

// Initialize port settings
// Some parts of the function brought from mipslabmain.c
void ports_init() {
	// Input pins
	TRISFSET = BIT(1);	// Set button 1 as input
	TRISDSET = BIT(7) | BIT(6) | BIT(5); // Set button 2,3,4 as input

	TRISECLR = 0xFF; // Set lights as output
	PORTE = 0x0; // Turn off lights

	T2CONSET = BIT(15); // Start timer 2

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
	TRISGCLR = BIT(9) | BIT(8) | BIT(6); // Set output for RG9, MOSI & clock
	TRISDCLR = BIT(4); // Set output for CS
	TRISFCLR = BIT(6) | BIT(5) | BIT(4); // Set output for SCK1, SCL2, D/C
	ODCB = 0x0;
	ODCD = 0x0;
	ODCF = 0x0;
	ODCG = 0x0; // Open-drain

	TRISBCLR = BIT(4) | BIT(8) | BIT(10) | BIT(12); // Set output for SCK, MOSI, CS & RST when using SW SPI

	// SPI settings
	SPI2BRG = 4;			// Baud rate
	SPI2STATCLR = BIT(6);	// SPIROV = 0	(no overflow has occurred)
	SPI2CON = 0;			// Clear SPI2CON (really necessary?)
	// SPI2CONSET = BIT(10);	// MODE16 = 1 	(16-bit data width)
	SPI2CONSET = BIT(6);	// CKP = 1 		(idle on high level)
	SPI2CONSET = BIT(5); 	// MSTEN = 1 	(master mode)
	SPI2CONSET = BIT(15);	// ON = 1 		(switch on SPI module)
}

// Initialize settings on the extern color display
void display_init() {
	quicksleep(1000000);
	DISPLAY_COMMAND_MODE;
	spi_send(0x28); // Display off
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
	spi_send(0x29); // Display on
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

// Set up chipKIT display
void oled_display_init(void) {
	DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE); // Display OFF
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D); // Charge pump command
	spi_send_recv(0x14); // Enable
	
	spi_send_recv(0xD9); // Pre-charge period
	spi_send_recv(0xF1); // Phase 1 period: 1 DCLK, Phase 2 period: 15 DCLK
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1); // Segment re-map (col 127 -> SEG0)
	spi_send_recv(0xC8); // COM output scan direction (reversed)
	
	spi_send_recv(0xDA); // COM pins hardware configuration
	spi_send_recv(0x20); // Sequential COM, enable left/right remap
	
	spi_send_recv(0xAF); // Display ON
}

void update_oled_display(uint8_t display_buf[DISPLAY_WIDTH][DISPLAY_HEIGHT]) {
	// Loop through page segments 1-4 which consists of 8 rows of pixels each
	for (int i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22); // Set page address
		spi_send_recv(i); // Start
		// spi_send_recv(i); // End??
		
		spi_send_recv(0 & 0xF); // Column start (lower nibble)
		spi_send_recv(0x10 | ((0 >> 4) & 0xF)); // Column start (higher nibble)
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for (int x = 0; x < DISPLAY_WIDTH; x++) {
			// Bits D0-D7 represents a 8 pixel long column of one "page"
			uint8_t packet = 0;
			for (int j = 0; j < 8; j++)
				packet |= display_buf[x][8*i + j] << j;

			spi_send_recv(packet); // Write pixels
		}
	}
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

void clear(uint8_t display_buf[DISPLAY_WIDTH][DISPLAY_HEIGHT]) {
	for (int x = 0; x < DISPLAY_WIDTH; x++)
		for (int y = 0; y < DISPLAY_WIDTH; y++)
			display_buf[x][y] = 0;
}

int main() {
	ports_init();
	// display_init();
	oled_display_init();

	player p = { MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f, 0.0f };

	// color column_buf[DISPLAY_HEIGHT];
	uint8_t display_buf[DISPLAY_WIDTH][DISPLAY_HEIGHT];


	// Game loop
	while (1) {
		int startTime = TMR2;
		clear(display_buf);
		// Test
		// for (int y = 0; y < DISPLAY_HEIGHT; y++) {
		// 	for (int x = 0; x < DISPLAY_WIDTH; x++) {
		// 		color pixel_color = { 0, 0, 0 };
				
		// 		if (x % 2 == 0) {
		// 			pixel_color.r = 0xFF;
		// 			pixel_color.g = 0xFF;
		// 			pixel_color.b = 0xFF;
		// 		}

		// 		DISPLAY_COMMAND_MODE;
		// 		spi_send(0x2C); // Memory write command
		// 		DISPLAY_DATA_MODE;
		// 		spi_send(0x0);
		// 		spi_send(0x0);
		// 	}
		// }

		float sinAngle = sinf(p.facingAngle);
		float cosAngle = cosf(p.facingAngle);
		
		// Execute raycasting logic and draw for each column on the screen
		for (int x = 0; x < DISPLAY_WIDTH; x++) {
			render_column(display_buf[x], x, p, sinAngle, cosAngle);

			// update_display(column_buf, x);
		}
		update_oled_display(display_buf);

		float deltaTime = (TMR2 - startTime) / 80000000 * 1000; // in milliseconds
		
		control_player(&p, sinAngle, cosAngle, deltaTime);
	}

    return 0;
}