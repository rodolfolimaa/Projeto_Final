// buttons.c
#include "include/buttons.h"
#include "pico/stdlib.h"

static bool last_a = true;
static bool last_b = true;

void buttons_init() {
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
}

bool button_a_pressed() {
    bool current = gpio_get(BUTTON_A);
    if (!current && last_a) {
        sleep_ms(50);
        last_a = current;
        return true;
    }
    last_a = current;
    return false;
}

bool button_b_pressed() {
    bool current = gpio_get(BUTTON_B);
    if (!current && last_b) {
        sleep_ms(50);
        last_b = current;
        return true;
    }
    last_b = current;
    return false;
}
