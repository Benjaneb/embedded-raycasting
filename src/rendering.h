#ifndef RENDERING
#define RENDERING

#include <stdint.h>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32
#define MAP_WIDTH 16
#define MAP_HEIGHT 16

typedef struct {
	uint8_t r, g, b;
} color;

typedef struct {
	float x, y, facingAngle;
} player;

int is_wall(float x, float y);

void render_column(uint8_t column_buf[DISPLAY_WIDTH][DISPLAY_HEIGHT], player p, float sinAngle, float cosAngle);

#endif