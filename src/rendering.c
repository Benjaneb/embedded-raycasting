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



void draw_vertical_line(int column_buf[DISPLAY_HEIGHT], int start_y, int end_y) {
    for (int y = start_y; y <= end_y; y++) {
        column_buf[y] = 1; // Turn on the pixel
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
            int start_y = DISPLAY_HEIGHT/2 - wall_height/2;
            int end_y = DISPLAY_HEIGHT/2 + wall_height/2;
            draw_vertical_line(column_buf, start_y, end_y);
            break;
        }    
    
    }
}

int is_wall(float x, float y) {
    return map[(int)floor(y)][(int)floor(x)] == '#';
}

void move_player(player *p, float dx, float dy) {
    float new_x = p->x + dx;
    float new_y = p->y + dy;

    if (!is_wall(new_x, new_y)) {
        p->x = new_x;
        p->y = new_y;
    }
}