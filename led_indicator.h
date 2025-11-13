#ifndef LED_INDICATOR_H
#define LED_INDICATOR_H

#include <Arduino.h>
#include "config.h"

void led_init();
void led_turn_off_all();
void led_indicate_discharge();
void led_indicate_charge();
void led_indicate_waiting();
void led_indicate_charge_done();

#endif // LED_INDICATOR_H
