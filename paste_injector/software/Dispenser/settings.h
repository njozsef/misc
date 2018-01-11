/*
 * Setings file by Jozsef Nagy
 * 
 * 
 The MIT License (MIT)
 
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

 * feature (future)
 * 0.91 Inch 128x32 IIC I2C Blue OLED LCD Display DIY Module SSD1306 Driver IC DC 3.3V 5V For Arduino PIC
*/

//#define OLED_128x32_ADAFRUIT_SCREENS

  #define STATE_IDLE 0
  #define STATE_FORWARD 1
  #define STATE_BACKWARD 3
  #define STATE_SLEEP 4
 // new feat
  #define STATE_DOUBLEPRESS 7
  #define STATE_DOUBLELONGPRESS 8
  #define WELCOME_TEXT "H-Extruder"
  #define longPressTime 10000

// tactile switch
#define buttonUp 8
#define buttonMode 10
#define buttonDown 9
#define KEY_DEBOUNCE 200

// Speeds in steps/sec.
#define kBackwardSpeed 700

//two button (F/R)  (pushbutton active LOW)
#define kForwardButtonPin 7
#define kBackwardButtonPin 6



// Seconds to wait before force entering screensaver
#define SCREENSAVER_TIMEOUT 30
#define START_STATE 0



// Arduino pin for nnboard LED. For debugging. Active high.
const int kLedPin = 13;

#ifndef OLED_128x32_ADAFRUIT_SCREENS
// Arduino potentiometer analog input pin.
static const int kPotPin = A0;
#endif

// After this idle time, the power to the motor is
// disconnected. 
static const uint32_t kSleepTimeMillis =20*1000;

// Time in millis since entering the current state.
static uint32_t current_state_start_time_millis = 0;



// Consts of a single segment of the pot value mapping function.
struct MapSegment {
  const int inMin;
  const int inMax;
  const int outMin;
  const int outMax;
};

// Consts for all the segments of the pot value mapping function.  (if no display is used))
static const MapSegment kMapSegments[] = {
  {  0,   128,  1,    2},
  {128,   256,  2,    4},
  {256,   384,  4,   10},
  {384,   512, 10,   22},
  {512,  640,  22,   48},
  {640,  768,  48,  105},
  {768,  896, 105,  229},
  {896, 1023, 229,  500},
};



#define EEPROM_ADR_STATE 0
#define EEPROM_ADR_TUNE 1
#define EEPROM_ADR_RSSI_MIN_A_L 2
#define EEPROM_ADR_RSSI_MIN_A_H 3
#define EEPROM_ADR_RSSI_MAX_A_L 4
#define EEPROM_ADR_RSSI_MAX_A_H 5

#define EEPROM_ADR_WELCOME_TEXT 20

