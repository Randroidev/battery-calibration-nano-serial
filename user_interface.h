#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <Arduino.h>

void ui_show_main_menu();
void ui_prompt_for_cycles();
void ui_prompt_invalid_cycles();
int ui_read_integer();
void ui_print_message(const String& message, bool new_line = true);
void ui_print_param(const String& key, const String& value);

#endif // USER_INTERFACE_H
