#include "motor.h"

// Using port D.
#define MOTOR_PORT      (PORTB)
#define MOTOR_PORT_DDR  (DDRB)

// Using a single port for all 4 motor pins. Connect
// motor's red wire to +5V.
//
// Motor blue wire. Arduino pin 12.
static const uint8_t kPin1Mask = (1 << 4);  

// Motor pink wire. Arduino pin 11.
static const uint8_t kPin2Mask = (1 << 3); 

 // Motor yellow wire. Arduino pin 10.
static const uint8_t kPin3Mask = (1 << 2); 

 // Motor orange wire. Arduino pin 9.
static const uint8_t kPin4Mask = (1 << 1);  

// Activated pins in each steps throughout one cycle.
static const uint8_t kStepTable[] = {
  kPin1Mask,               // step 0
  kPin1Mask | kPin2Mask,   // step 1
  kPin2Mask,               // step 2
  kPin2Mask | kPin3Mask,   // step 3
  kPin3Mask,               // step 4
  kPin3Mask | kPin4Mask,   // step 5
  kPin4Mask,               // step 6
  kPin4Mask | kPin1Mask     // step 7  
};

// A union of all pin masks.
static const uint8_t kAllPinsMask = kPin1Mask | kPin2Mask | kPin3Mask | kPin4Mask;

// We consider only the last three bits of the step counter (8 steps per cycle).
static const uint8_t kStepMask = 0x7;

// Set 4 pins per their respective bits in values. All other bits of values
// are ignored.
void Motor::updatePins(uint8_t values) {
  cli();
  MOTOR_PORT = ((MOTOR_PORT & ~kAllPinsMask) | values); 
  sei();  
}

void Motor::setStep(uint8_t step) {
  const uint8_t step_index = step & kStepMask;
  updatePins(kStepTable[step_index]);
}

bool Motor::isNonZeroSpeed() {
  return step_time_micros != 0;
}

void Motor::doStep() {
  if (is_forward) {
    step_counter++;
  } else {
    step_counter--;
  }
  last_step_micros = micros();
  setStep(step_counter);
}

Motor::Motor() {
  step_counter = 0;
  last_step_micros = 0;
  is_forward = true;
  // If 0 indicates zero speed.
  step_time_micros = 0;


  MOTOR_PORT_DDR |= kAllPinsMask;
  // Pins are off.
  updatePins(0);  
  last_step_micros = micros(); 
  
  setStep(step_counter);
}

void Motor::loop() {
  if (!step_time_micros) {
    last_step_micros = micros();
    return;
  }
  
  // TODO: cach the target time in micros?
  if ((micros() - last_step_micros) >= step_time_micros) {
    doStep();
  }
}

// 0 indicates stop.
void Motor::setSpeed(uint16_t steps_per_sec, boolean forward) {
  // Turn on the motor in case it was off.
  setStep(step_counter);
  
  if (!steps_per_sec) {
    step_time_micros = 0;
    is_forward = forward;
    return;  
  }

  step_time_micros = (1000000L / steps_per_sec);
  is_forward = forward;
}

void Motor::sleep() {
  step_time_micros = 0;
  updatePins(0);
}

