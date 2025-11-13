#ifndef PROCESS_CONTROLLER_H
#define PROCESS_CONTROLLER_H

#include "battery_manager.h"

enum class Process {
  IDLE,
  CHARGE,
  DISCHARGE,
  CALIBRATION,
  DEMO
};

class ProcessController {
public:
  ProcessController(BatteryManager& bat_manager);
  void init();
  void start_charge();
  void start_discharge();
  void start_calibration(int cycles);
  void start_demo(int cycles);
  void stop_process();
  void update();
  bool is_busy() const;

private:
  BatteryManager& battery;
  Process current_process;
  int total_cycles;
  int current_cycle;
  enum class CalibrationStep {
    PRE_CALIB_CHARGING,
    PRE_CALIB_WAITING,
    START_DISCHARGE,
    DISCHARGING,
    POST_DISCHARGE_WAIT,
    START_CHARGE,
    CHARGING,
    POST_CHARGE_WAIT
  };
  CalibrationStep calib_step;
  unsigned long process_start_time;
  unsigned long step_start_time;
  unsigned long last_battery_read;
  int consecutive_read_errors;
  void control_relays(bool charge, bool discharge);
  void update_charge();
  void update_discharge();
  void update_calibration_or_demo(bool is_demo);
  void periodic_battery_check(bool full_report, bool is_demo = false);
};

#endif // PROCESS_CONTROLLER_H
