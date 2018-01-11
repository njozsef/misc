/*
 * 
 * 
 * 
 * original code by Zapta
 **************************
 * added Oled display
 * added complete menu
 * added vaccuum mode
 **************************


The MIT License (MIT)

Copyright (c) 2018 Jozsef Nagy

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <avr/pgmspace.h>
#include <EEPROM.h>
#include "motor.h"
#include "settings.h"


#ifdef OLED_128x32_ADAFRUIT_SCREENS
    #include <Adafruit_SSD1306.h>
     #include <Adafruit_GFX.h>
    #include <Wire.h>
    #include <SPI.h>
#endif


//*** longpressbuttons features (dev. progress for oled menus)
long buttonTimer = 0;
boolean buttonActive = false;

// Controller the solder paste injector.

// For motor pin connection see motor.ino.
//----------------------------------------

//init
static uint8_t state = STATE_IDLE;

char welcome_text[10];


Motor motor = Motor();

// Arduino initialization function.
void setup() {
  Serial.begin(115200);
  Serial.println("Paste injector v0.0.1");
  Serial.println("Original code by Zapta.");
 
  pinMode(kLedPin, OUTPUT);

  //Pushbuttons
  pinMode(kForwardButtonPin, INPUT_PULLUP);
  pinMode(kBackwardButtonPin, INPUT_PULLUP);
  // Tactile control
  pinMode(buttonUp, INPUT);
  digitalWrite(buttonUp, INPUT_PULLUP);
  pinMode(buttonMode, INPUT);
  digitalWrite(buttonMode, INPUT_PULLUP);
  pinMode(buttonDown, INPUT);
  digitalWrite(buttonDown, INPUT_PULLUP);

    // use values only of EEprom is not 255 = unsaved
    uint8_t eeprom_check = EEPROM.read(EEPROM_ADR_STATE);
    if(eeprom_check == 255) // unused
    {
        // save 8 bit
        EEPROM.write(EEPROM_ADR_STATE,START_STATE);
      /*  EEPROM.write(EEPROM_ADR_TUNE,CHANNEL_MIN_INDEX);
        EEPROM.write(EEPROM_ADR_BEEP,settings_beeps);
        EEPROM.write(EEPROM_ADR_ORDERBY,settings_orderby_channel);
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MIN_A_L,lowByte(RSSI_MIN_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MIN_A_H,highByte(RSSI_MIN_VAL));
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MAX_A_L,lowByte(RSSI_MAX_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MAX_A_H,highByte(RSSI_MAX_VAL));
*/
        // save default welcome_text
        strcpy(welcome_text, WELCOME_TEXT); // load callsign
        for(uint8_t i = 0;i<sizeof(welcome_text);i++) {
            EEPROM.write(EEPROM_ADR_WELCOME_TEXT+i,welcome_text[i]);
        }
    }



  
  motor.begin();
  motor.setSpeed(0, true);
  Serial.println("Refactored ny Jozsef Nagy.");
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

