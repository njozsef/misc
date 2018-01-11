#include "motor.h"
#include "settings.h"


#ifdef OLED_128x32_ADAFRUIT_SCREENS
    #include <Adafruit_SSD1306.h>
     #include <Adafruit_GFX.h>
    #include <Wire.h>
    #include <SPI.h>
#endif

// Controller the solder paste injector.

// For motor pin connection see motor_io.cpp.
// Arduino initialization function.
void setup() {
  Serial.begin(115200);
  Serial.println("Paste injector v0.0.1");
  Serial.println("Original code by Zapta.");
 
  pinMode(kLedPin, OUTPUT);

  //PUSHBUTTONS
  pinMode(kForwardButtonPin, INPUT_PULLUP);
  pinMode(kBackwardButtonPin, INPUT_PULLUP);

  // Tactile control
  pinMode(buttonUp, INPUT);
  digitalWrite(buttonUp, INPUT_PULLUP);
  pinMode(buttonMode, INPUT);
  digitalWrite(buttonMode, INPUT_PULLUP);
  pinMode(buttonDown, INPUT);
  digitalWrite(buttonDown, INPUT_PULLUP);
  pinMode(buttonSave, INPUT);
  digitalWrite(buttonSave, INPUT_PULLUP);



  
  motor.begin();
  motor.setSpeed(0, true);
}






// Arduino main loop function.
void loop() {  
  // Update the motor outputs as needed.
  motor.loop();

  // TODO: have a more interesting LED pattern. Currently it's on iff
  // in a state that turns the motor.
  digitalWrite(kLedPin, motor.isNonZeroSpeed());






  // Here when last motion operation is completed.
  switch (state) {
    case STATE_IDLE:
      if (millisInCurrentState() >= kSleepTimeMillis) {
        motor.sleep();
        setState(STATE_SLEEP);
        return;
      }
      
      // Handle buttons unly after minimal debouncing delay.
      if (!debouncingDelay()) {
        return;
      }

      
      // Handle Forward button press.
      if (isForwardButtonPressed()) {
        motor.setSpeed(readPotAsSpeed(), true);
        setState(STATE_FORWARD);
        return;
      }
      // Handle Backward button press.
      if (isBackwardButtonPressed()) {
        motor.setSpeed(kBackwardSpeed, false);
        setState(STATE_BACKWARD);
        return;
      }
      break;

    case STATE_FORWARD:
      // Handle Forward button release.
      if (!isForwardButtonPressed() && debouncingDelay()) {
        motor.setSpeed(0, true);
        setState(STATE_IDLE);
        return;
      }
      // Handle pot changes.
      if (millisSinceLastPotRead() >= 250) {
        motor.setSpeed(readPotAsSpeed(), true);
      }
      break;

    case STATE_BACKWARD:
      // Handle Backward button release.
      if (!isBackwardButtonPressed() && debouncingDelay()) {
        motor.setSpeed(0, true);
        setState(STATE_IDLE);
        return;
      }
      break;

    case STATE_SLEEP:
      if (isForwardButtonPressed() || isBackwardButtonPressed()) {
        motor.setSpeed(0, false);
        setState(STATE_IDLE);
        return;  
      }
      break;

    default:
      // Should never happend.
      Serial.println("Unknown state");
      // This also prints the unknown state.
      setState(STATE_IDLE);
  }
}



















// A common function to change state.
static void setState(uint8_t newState) {
  if (newState != state) {
    Serial.print(state);
    Serial.print(" --> ");
    Serial.println(newState);
    state = newState;
    current_state_start_time_millis = millis();
  }
}

static inline uint32_t millisInCurrentState() {
  return (millis() - current_state_start_time_millis);
}

// Using this as a poor man debouncing in states that we entered
// due to a button action.
static inline boolean debouncingDelay() {
   return millisInCurrentState() >= 25;
}

// NUmber of segments in kMapSegments.
const int kNumMapSegments = sizeof(kMapSegments) / sizeof(kMapSegments[0]);

// Map pot value to speed. We use an aproximation of a logarithmic
// function using the linear segments in kMapSegments.
int mapPotValue(int potValue) {
  potValue = constrain(potValue, 0, 1023);
  for (int i = 0; i < kNumMapSegments; i++) {
    const MapSegment& s = kMapSegments[i];
    if (potValue <= s.inMax) {
      // TODO: precompute dIn and dOut.
      return (int) (((long)potValue - s.inMin) * (s.outMax - s.outMin) / (s.inMax - s.inMin)) + s.outMin;
    }
  }
  return kMapSegments[kNumMapSegments - 1].outMax;
}

// These two are updated each time readPostAsSpeed() is called.
static unsigned long timeLastPotReadMillis = 0;
static int lastPotValueAsSpeed = 0;

// Read pot value and return it mapped to speed.
static int readPotAsSpeed() {
  timeLastPotReadMillis = millis();
  const int potValue = analogRead(kPotPin);
  lastPotValueAsSpeed = mapPotValue(potValue);
  Serial.print("S:");
  Serial.println(lastPotValueAsSpeed);
  return lastPotValueAsSpeed;
}

inline long millisSinceLastPotRead() {
  return millis() - timeLastPotReadMillis;
}

inline bool isForwardButtonPressed() {
  // TODO: debounce.
  //
  // Active low.
  return !digitalRead(kForwardButtonPin);
}

inline bool isBackwardButtonPressed() {
  // TODO: debounce.
  //
  // Active low.
  return !digitalRead(kBackwardButtonPin);
}

