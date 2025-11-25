#include "input_manager.h"
#include "hardware/gpio.h"
#include <pico/volatile.h>

#define ENCODER_A_PIN 8
#define ENCODER_B_PIN 9
#define ENCODER_BTN_PIN 10
#define BACK_BTN_PIN 13

static volatile int encoder_pos = 0;
static volatile bool encoder_btn_pressed = false;
static volatile bool back_btn_pressed = false;

static void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == ENCODER_A_PIN) {
        if (gpio_get(ENCODER_B_PIN)) {
            encoder_pos--;
        } else {
            encoder_pos++;
        }
    } else if (gpio == ENCODER_BTN_PIN) {
        encoder_btn_pressed = true;
    } else if (gpio == BACK_BTN_PIN) {
        back_btn_pressed = true;
    }
}

void input_init() {
    gpio_init(ENCODER_A_PIN);
    gpio_set_dir(ENCODER_A_PIN, GPIO_IN);
    gpio_pull_up(ENCODER_A_PIN);

    gpio_init(ENCODER_B_PIN);
    gpio_set_dir(ENCODER_B_PIN, GPIO_IN);
    gpio_pull_up(ENCODER_B_PIN);

    gpio_init(ENCODER_BTN_PIN);
    gpio_set_dir(ENCODER_BTN_PIN, GPIO_IN);
    gpio_pull_up(ENCODER_BTN_PIN);

    gpio_init(BACK_BTN_PIN);
    gpio_set_dir(BACK_BTN_PIN, GPIO_IN);
    gpio_pull_up(BACK_BTN_PIN);

    gpio_set_irq_enabled_with_callback(ENCODER_A_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled(ENCODER_BTN_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BACK_BTN_PIN, GPIO_IRQ_EDGE_FALL, true);
}

UserAction input_get_action() {
    static int last_encoder_pos = 0;

    if (encoder_pos > last_encoder_pos) {
        last_encoder_pos = encoder_pos;
        return UserAction::ENCODER_UP;
    }
    if (encoder_pos < last_encoder_pos) {
        last_encoder_pos = encoder_pos;
        return UserAction::ENCODER_DOWN;
    }
    if (encoder_btn_pressed) {
        encoder_btn_pressed = false;
        return UserAction::ENCODER_PRESS;
    }
    if (back_btn_pressed) {
        back_btn_pressed = false;
        return UserAction::BACK_PRESS;
    }

    return UserAction::NONE;
}
