#ifndef RENDERING
#define RENDERING

#include <stdint.h>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32
#define MAP_WIDTH 16
#define MAP_HEIGHT 16
#define PI 3.14159265f

typedef struct {
	uint8_t r, g, b;
} color;

typedef struct {
	float x, y, facingAngle, breathing;
} player;

char map[MAP_HEIGHT][MAP_WIDTH];

void place_goal();

int is_goal(float x, float y);

int is_wall(float x, float y);

void render(uint8_t column_buf[DISPLAY_WIDTH][DISPLAY_HEIGHT], player p, float sinAngle, float cosAngle);

#endif