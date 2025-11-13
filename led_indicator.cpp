#include "led_indicator.h"

const int LED_PINS[] = {
  LED_PIN_DISCHARGE,
  LED_PIN_WAITING,
  LED_PIN_CHARGE,
  LED_PIN_CHARGE_DONE
};
const int NUM_LEDS = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

void led_init() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
}

void led_turn_off_all() {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], LOW);
  }
}

void _set_active_led(int active_pin) {
  led_turn_off_all();
  digitalWrite(active_pin, HIGH);
}

void led_indicate_discharge() {
  _set_active_led(LED_PIN_DISCHARGE);
}

void led_indicate_charge() {
  _set_active_led(LED_PIN_CHARGE);
}

void led_indicate_waiting() {
  _set_active_led(LED_PIN_WAITING);
}

void led_indicate_charge_done() {
  _set_active_led(LED_PIN_CHARGE_DONE);
}
