/*
 * Setings file by Jozsef Nagy
 * 
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

  #define longPressTime 10000

// tactile switch
#define buttonUp 2
#define buttonMode 3
#define buttonDown 4
#define KEY_DEBOUNCE 200

// Speeds in steps/sec.
#define kBackwardSpeed 700

//two button (F/R)  (pushbutton active LOW)
#define kForwardButtonPin 7
#define kBackwardButtonPin 6


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


#ifndef OLED_128x32_ADAFRUIT_SCREENS
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
#endif

