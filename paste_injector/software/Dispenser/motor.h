#ifndef motor_h
#define motor_h

#include "Arduino.h"

class Motor 
{
  public:  
    Motor();
    void begin() {};
    void loop();
    void sleep();
  
    // 0 indicates stop. Turns on motor is it's off.
    void setSpeed(uint16_t steps_per_sec, boolean is_forward);
    bool isNonZeroSpeed();
  
  private:
    void updatePins(uint8_t values);
    void setStep(uint8_t step);
    void doStep();
    uint8_t step_counter;
    uint32_t last_step_micros;
    boolean is_forward;
    uint32_t step_time_micros;

};  // motor

#endif  // ifdef motor_h

