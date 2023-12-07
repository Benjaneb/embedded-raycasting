#ifndef RENDERING
#define RENDERING

#include <stdint.h>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 128

typedef struct {
	uint8_t r, g, b;
} color;

typedef struct {
	float x, y, facingAngle;
} player;

void render_column(color column_buf[DISPLAY_HEIGHT], int screenX, player p, float sinAngle, float cosAngle);

#endif