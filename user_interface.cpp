#include "user_interface.h"
#include "config.h"

void ui_show_main_menu() {
  Serial.println(F("\n===== Main Menu ====="));
  Serial.println(F("1. Run Calibration"));
  Serial.println(F("2. Read Battery Data"));
  Serial.println(F("3. Start Charge"));
  Serial.println(F("4. Start Discharge"));
  Serial.println(F("5. Demo"));
  Serial.print(F("Enter your choice: "));
}

void ui_prompt_for_cycles() {
  Serial.print(F("Enter cycles count [1...5] (0 to return to menu): "));
}

void ui_prompt_invalid_cycles() {
  Serial.println(F("Invalid input. Please enter a number between 1 and 5, or 0 to exit."));
}

void ui_print_message(const String& message, bool new_line) {
    if (new_line) {
        Serial.println(message);
    } else {
        Serial.print(message);
    }
}

void ui_print_param(const String& key, const String& value) {
    Serial.print(F("  "));
    Serial.print(key);
    Serial.print(F(": "));
    Serial.println(value);
}

int ui_read_integer() {
  String input_string = "";
  char in_char;
  
  while (true) {
    if (Serial.available() > 0) {
      in_char = Serial.read();
      Serial.print(in_char); 

      if (isDigit(in_char)) {
        input_string += in_char;
      }
      else if (in_char == '\b' && input_string.length() > 0) {
        input_string.remove(input_string.length() - 1);
        Serial.print(F(" \b")); 
      }
      else if (in_char == '\r' || in_char == '\n') {
        if (input_string.length() > 0) {
          Serial.println();
          return input_string.toInt();
        }
        else {
           Serial.println();
        }
      }
    }
  }
}
