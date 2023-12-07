#include <pic32mx.h>
#include <rendering.h>
#include <math.h>

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

void move_player(player p, float sinAngle, float cosAngle) {
    static const float turningSpeed = 0.1f;
    static const float movementSpeed = 0.1f;

    // Buttons are physically arranged: BTN4 BTN3 BTN2 BTN1

    // Turn right
    if (get_button1()) {
        p.facingAngle -= turningSpeed;
    }

    // Move backwards
    if (get_button2()) {
        p.x -= cosAngle * movementSpeed;
        p.y -= sinAngle * movementSpeed;
    }

    // Move forward
    if (get_button3()) {
        p.x += cosAngle * movementSpeed;
        p.y += sinAngle * movementSpeed;
    }

    // Turn left
    if (get_button4()) {
        p.facingAngle += turningSpeed;
    }
}