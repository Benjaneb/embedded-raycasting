#include "rendering.h"

#define PI 3.14159265f

#define MAX_DISTANCE 10
#define MAP_WIDTH 16
#define MAP_HEIGHT 16

// # is wall
// . is empty space
static const char map[MAP_HEIGHT][MAP_WIDTH] = {
    '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
    '#', '.', '.', '#', '.', '.', '.', '.', '.', '#', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '#', '.', '.', '.', '.', '.', '#', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '#', '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '.', '.', '.', '.', '#', '#', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '.', '.', '.', '.', '#', '#', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '#', '#', '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
};

void render_column(color column_buf[DISPLAY_HEIGHT], int screenX, player p, float sinAngle, float cosAngle) {
    static const float fov = 0.25f * PI;

    // Psuedo code for vector solution:
    
    // y = tan(FOV / 2) * DISPLAY_WIDTH/2
    // direction = (x - DISPLAY_WIDTH/2, y)
    // marching_position = (player.x, player.y)
    // for distance = 1; distance < MAX_DISTANCE; distance += STEP_SIZE
    //     marching_position += distance * direction
    //     if map[floor(marching_position.y)][floor(marching_position.x)] == '#'
    //         wall_height = DISPLAY_HEIGHT / distance
    //         draw line from (x, DISPLAY_HEIGHT/2 - wall_height/2) to (x, DISPLAY_HEIGHT/2 + wall_height/2)
}