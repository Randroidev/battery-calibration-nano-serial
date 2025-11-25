#ifndef CONFIG_H
#define CONFIG_H

// --- Pin Definitions ---
// LEDs for status indication
const int LED_PIN_DISCHARGE = A0; // Indicates discharge process
const int LED_PIN_WAITING = A1;   // Indicates waiting period
const int LED_PIN_CHARGE = A2;    // Indicates charge process
const int LED_PIN_CHARGE_DONE = A3; // Indicates charge completion wait

// Relays for charge and discharge control
const int RELAY_PIN_CHARGE = 5;
const int RELAY_PIN_DISCHARGE = 7;

// --- Relay Logic ---
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

// --- Timing Constants (in milliseconds) ---
const unsigned long BATTERY_READ_INTERVAL_MS = 15000; // 15 seconds
const unsigned long CALIBRATION_PRE_CHARGE_WAIT_MS = 1800000; // 30 minutes
const unsigned long CALIBRATION_CHARGE_WAIT_MS = 3600000; // 1 hour
const unsigned long CALIBRATION_DISCHARGE_WAIT_MS = 18000000; // 5 hours

// --- Demo Mode Timings ---
const unsigned long DEMO_PROCESS_DURATION_MS = 3000; // 3 seconds
const unsigned long DEMO_WAIT_DURATION_MS = 3000;    // 3 seconds

// --- Battery Communication ---
const int BATTERY_CONNECT_RETRIES = 3;
const bool USE_ADDITIONAL_CONDITIONS = true;

// --- Serial Communication ---
const int SERIAL_BAUD_RATE = 9600;

#endif // CONFIG_H
