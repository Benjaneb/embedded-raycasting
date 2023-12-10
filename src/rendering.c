#include <math.h>
#include "rendering.h"

#define MAX_DISTANCE 10
#define MAX_STEPS 120

#define STEP_SIZE 0.1f

typedef struct {
    float x;
    float y;
} vector;

// # is wall
// . is empty space
char map[MAP_HEIGHT][MAP_WIDTH] = {
    '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
    '#', '.', '.', '#', '.', '.', '.', '.', '.', '#', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '#', '.', '.', '.', '.', '.', '#', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '#', '.', '.', '.', '#', '#', '#', '#', '.', '.', '.', '.', '#',
    '#', '.', '#', '#', '.', '.', '.', '.', '#', '.', '.', '.', '.', '.', '.', '#',
    '#', '.', '.', '.', '.', '#', '.', '.', '.', '.', '.', '.', '#', '.', '.', '#',
    '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '#', '.', '#', '.', '.', '#',
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

int rand() {
    static unsigned int state = 123456789; // Seed value
    const unsigned int a = 1103515245;     // Multiplier
    const unsigned int c = 12345;          // Increment
    const unsigned int m = 2147483648;     // Modulus (2^31)

    state = (a * state + c) % m;

    return state % MAP_HEIGHT;
}

float clamp(float x, float min, float max) {
  const float temp = x < min ? min : x;
  return temp > max ? max : temp;
}

void place_goal() {
    while (1) {
        int x = rand();
        int y = rand();

        if (!is_wall(x, y)) {
            map[y][x] = 'x';
            break;
        }
    }
}

void draw_goal(uint8_t column_buf[DISPLAY_HEIGHT], int start_y, int end_y) {
    // Turn on every pixel from start_y to end_y
    for (int y = start_y; y <= end_y; y++) {
        column_buf[y] = y % 2 == 0;
    }
}

void draw_vertical_line(uint8_t column_buf[DISPLAY_HEIGHT], int start_y, int end_y) {
    // Turn on every pixel from start_y to end_y
    for (int y = start_y; y <= end_y; y++) {
        column_buf[y] = 1;
    }
}

// Check if a position is a goal
int is_goal(float x, float y) {
    int flooredX = (int)floorf(x);
    int flooredY = (int)floorf(y);

    // Return false if position is outside map
    if (flooredX < 0 || flooredX >= MAP_WIDTH || 
        flooredY < 0 || flooredY >= MAP_HEIGHT)
        return 0;
    
    return map[flooredY][flooredX] == 'x';
}

// Check if a position is a wall
int is_wall(float x, float y) {
    int flooredX = (int)floorf(x);
    int flooredY = (int)floorf(y);

    // Return false if position is outside map
    if (flooredX < 0 || flooredX >= MAP_WIDTH || 
        flooredY < 0 || flooredY >= MAP_HEIGHT)
        return 0;
    
    return map[flooredY][flooredX] != '.';
}

// Render a single column with raycasting
void render(uint8_t column_buf[DISPLAY_WIDTH][DISPLAY_HEIGHT], player p, float sinAngle, float cosAngle) {
    int startX = -MAX_DISTANCE;
    int endX = MAX_DISTANCE;
    float deltaX = endX - startX;
    int column = 0;

    for (float x = startX; x < endX; x += deltaX / (float)DISPLAY_WIDTH) {
        column++;
        // Calculate the slope of the ray
        float slope = x / MAX_DISTANCE;

        // March in the direction until hitting a wall or reaching MAX_DISTANCE
        for (float distance = 0; distance < MAX_DISTANCE; distance += STEP_SIZE) {
            // Move in x and y directions
            float rayX = slope * distance;
            float rayY = distance;
            float rotatedRayX = cosAngle * rayX - sinAngle * rayY + p.x;
            float rotatedRayY = sinAngle * rayX + cosAngle * rayY + p.y;

            if (is_wall(rotatedRayX, rotatedRayY)) {
                float wall_height = DISPLAY_HEIGHT / distance;
                int start_y = DISPLAY_HEIGHT / 2.0f - wall_height / 2.0f + p.breathing;
                int end_y = DISPLAY_HEIGHT / 2.0f + wall_height / 2.0f + p.breathing;

                // Clamp start_y and end_y
                start_y = clamp(start_y, 0, DISPLAY_HEIGHT - 1);
                end_y = clamp(end_y, 0, DISPLAY_HEIGHT - 1);

                if (is_goal(rotatedRayX, rotatedRayY))
                    draw_goal(column_buf[column], start_y, end_y);
                else
                    draw_vertical_line(column_buf[column], start_y, end_y);
                break;
            }
        }
    }
}
