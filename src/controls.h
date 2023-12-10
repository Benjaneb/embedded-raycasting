#ifndef CONTROLS
#define CONTROLS

#include "rendering.h"

int get_button1();

int get_button2();

int get_button3();

int get_button4();

void move_player(player *p, float dx, float dy);

void control_player(player *p, float sinAngle, float cosAngle);

#endif