#ifndef BUTTONS_H
#define BUTTONS_H

#define BUTTON_A 5
#define BUTTON_B 6

#include <stdbool.h>

void buttons_init();
bool button_a_pressed();
bool button_b_pressed();

#endif