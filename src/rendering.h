#include <stdint.h>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 128

typedef struct {
	uint8_t r, g, b;
} color;

void render_column(color display_buf[DISPLAY_HEIGHT][DISPLAY_WIDTH]);