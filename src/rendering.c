#include "rendering.h"

#define PI 3.14159265f

#define MAX_DISTANCE 10
#define MAP_WIDTH 16
#define MAP_HEIGHT 16

#define STEP_SIZE 0.01f

typedef struct {
    float x;
    float y;
} vector;

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

void draw_line(int x1, int y1, int x2, int y2) {    // Bresenham's line algorithm

    int dx = x2 - x1;   // delta x
    int dy = y2 - y1;   // delta y

    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);  // number of steps to take

    float xIncrement = dx / (float)steps;
    float yIncrement = dy / (float)steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++) {  // draw line
        draw_pixel(x, y);   
        x += xIncrement;
        y += yIncrement;
    }
}

void render_column(color column_buf[DISPLAY_HEIGHT], int screenX, player p, float sinAngle, float cosAngle) {   // render a single column

    static const float fov = 0.25f * PI;            // field of view
    float y = tan(fov / 2) * DISPLAY_WIDTH/2;       // y value of the direction vector

    vector direction = {screenX - DISPLAY_WIDTH/2, y};  // direction vector
    vector marching_position = {p.x, p.y};              // position vector

    for (float distance = 1; distance < MAX_DISTANCE; distance += STEP_SIZE) {

        marching_position.x += distance * direction.x;
        marching_position.y += distance * direction.y;

        if (map[(int)floor(marching_position.y)][(int)floor(marching_position.x)] == '#') { // if we hit a wall

            float wall_height = DISPLAY_HEIGHT / distance;
            draw_line(screenX, DISPLAY_HEIGHT/2 - wall_height/2, screenX, DISPLAY_HEIGHT/2 + wall_height/2);
            break;
        }
    }
}