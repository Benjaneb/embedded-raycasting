#include <pic32mx.h>
#include <math.h>
#include "rendering.h"

int get_button1() {
    return PORTF & (1 << 1);
}

int get_button2() {
    return PORTD & (1 << 5);
}

int get_button3() {
    return PORTD & (1 << 6);
}

int get_button4() {
    return PORTD & (1 << 7);
}

void move_player(player *p, float dx, float dy) {
    float new_x = p->x + dx;
    float new_y = p->y + dy;

    // Check collision only in y direction
    if (!is_wall(p->x, new_y)) {
        p->y = new_y;
    }

    // Check collision only in x direction
    if (!is_wall(new_x, p->y)) {
        p->x = new_x;
    }
}

void control_player(player *p, float sinAngle, float cosAngle, float deltaTime) {
    const float turningSpeed = 0.3f;
    const float movementSpeed = 0.5f;

    // Buttons are physically arranged: BTN4 BTN3 BTN2 BTN1

    // Turn right
    if (get_button1()) {
        p->facingAngle -= turningSpeed;
    }

    // Move backwards
    if (get_button2()) {
        move_player(p, sinAngle * movementSpeed, -cosAngle * movementSpeed);
    }

    // Move forward
    if (get_button3()) {
        move_player(p, -sinAngle * movementSpeed, cosAngle * movementSpeed);
    }

    // Turn left
    if (get_button4()) {
        p->facingAngle += turningSpeed;
    }
}