#include "led_indicator.h"
#include "pico/stdlib.h"

#define LED_PIN_DISCHARGE 0
#define LED_PIN_WAITING   1
#define LED_PIN_CHARGE    2
#define LED_PIN_CHARGE_DONE 3

const uint led_pins[] = {LED_PIN_DISCHARGE, LED_PIN_WAITING, LED_PIN_CHARGE, LED_PIN_CHARGE_DONE};

void led_init() {
    for (int i = 0; i < 4; i++) {
        gpio_init(led_pins[i]);
        gpio_set_dir(led_pins[i], GPIO_OUT);
    }
}

void led_turn_off_all() {
    for (int i = 0; i < 4; i++) {
        gpio_put(led_pins[i], 0);
    }
}

void led_indicate_charge() {
    led_turn_off_all();
    gpio_put(LED_PIN_CHARGE, 1);
}

void led_indicate_discharge() {
    led_turn_off_all();
    gpio_put(LED_PIN_DISCHARGE, 1);
}

void led_indicate_waiting() {
    led_turn_off_all();
    gpio_put(LED_PIN_WAITING, 1);
}

void led_indicate_charge_done() {
    led_turn_off_all();
    gpio_put(LED_PIN_CHARGE_DONE, 1);
}
