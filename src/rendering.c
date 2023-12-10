#include <math.h>
#include "rendering.h"

#define PI 3.14159265f

#define MAX_DISTANCE 20
#define MAX_STEPS 120

#define STEP_SIZE 0.1f

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
    '#', '.', '.', '#', '.', '.', '.', '#', '#', '#', '#', '#', '.', '.', '.', '#',
    '#', '.', '#', '#', '.', '.', '.', '.', '#', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '.', '.', '#', '.', '.', '#', '.', '.', '.', '#', '.', '.', '#',
    '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#', '#', '#', '.', '.', '#',
    '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#', '.', '#', '.', '.', '#',
    '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#', '.', '#', '.', '.', '#',
    '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '#', '#', '#', '.', '.', '#', '#', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#',
    '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
};


float clamp(float x, float min, float max) {
  const float temp = x < min ? min : x;
  return temp > max ? max : temp;
}

void draw_vertical_line(uint8_t column_buf[DISPLAY_HEIGHT], int start_y, int end_y) {
    // Clamp start_y and end_y
    start_y = clamp(start_y, 0, DISPLAY_HEIGHT);
    end_y = clamp(end_y, 0, DISPLAY_HEIGHT);

    // Turn on every pixel from start_y to end_y
    for (int y = start_y; y <= end_y; y++) {
        column_buf[y] = 1;
    }
}

// Use a grid to store the map
int map_grid[MAP_WIDTH][MAP_HEIGHT];

// Initialize the grid with the map data
void init_map_grid() {
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            map_grid[x][y] = map[x][y] == '#';
        }
    }
}

// Check if a position is a wall using the grid
int is_wall(float x, float y) {
    int flooredX = (int)floorf(x);
    int flooredY = (int)floorf(y);

    // Return false if position is outside map
    if (flooredX < 0 || flooredX > MAP_WIDTH || 
        flooredY < 0 || flooredY > MAP_HEIGHT)
        return 0;
    
    return map_grid[flooredX][flooredY];
}

// Render a single column with raycasting
void render_column(uint8_t column_buf[DISPLAY_HEIGHT], int screenX, player p, float sinAngle, float cosAngle) {
    static const float fov = 0.45f * PI; // field of view based on 45 degrees

    // Calculate the slope of the ray
    float dx = cosAngle;
    float dy = sinAngle;
    float slope = dy / dx;
    float intercept = p.y - slope * p.x;

    // Determine the direction of the ray
    int stepX = dx > 0 ? 1 : -1;
    int stepY = dy > 0 ? 1 : -1;

    // Start at the player's position
    float x = p.x;
    float y = p.y;

    // March in the direction until hitting a wall or reaching MAX_DISTANCE
    for (int step = 0; step < MAX_STEPS; step++) {
        
        // Move in x and y directions
        x += stepX * STEP_SIZE;
        y += stepY * STEP_SIZE;

        if (is_wall(x, y)) {
            // Use Manhattan distance
            float distance = abs(x - p.x) + abs(y - p.y);
            float wall_height = DISPLAY_HEIGHT / (distance * 4.0f);
            int start_y = DISPLAY_HEIGHT / 2.0f - wall_height / 2.0f;
            int end_y = DISPLAY_HEIGHT / 2.0f + wall_height / 2.0f;
            draw_vertical_line(column_buf, start_y, end_y);
            break;
        }
    }

