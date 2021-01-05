/*
 * Set an interrupt event on the RTC and handle the interrupt on ESP32
 */

#include <M5Stack.h>

#include "M5_RTC_Module.h"

// The pin you want to connect the interrupt output of the RTC to
const uint8_t INT_PIN = 35;

// The below variables control what the alarm will be set to
const uint8_t ALM_MINUTE = 1;
const uint8_t ALM_HOUR = 10;
const uint16_t ALM_DATE_OR_WEEKDAY = 2;
const bool ALM_IS_WEEKDAY = false;
// Alarm when minutes match
const uint8_t ALM_MODE = 6;
/*********************************
  Set the alarm mode in the following way:
  0: When minutes, hours and weekday/date match (once per weekday/date)
  1: When hours and weekday/date match (once per weekday/date)
  2: When minutes and weekday/date match (once per hour per weekday/date)
  3: When weekday/date match (once per weekday/date)
  4: When hours and minutes match (once per day)
  5: When hours match (once per day)
  6: When minutes match (once per hour)
  7: All disabled – Default value
  If you want to set a weekday alarm (alm_isweekday = true), set 'alm_date_or_weekday' from 0 (Sunday) to 6 (Saturday)
  More informations can be found here: https://github.com/constiko/RV-3028_C7-Arduino_Library
********************************/

// Create M5_RTC_Mod instance
M5_RTC_Mod rtc_mod;

// Prototype of the ISR
void IRAM_ATTR isr();

void setup() {
    // Initialize the M5Stack object
    M5.begin();
    // clear screen
    M5.Lcd.fillScreen(BLACK);
    // set white text color with black background
    M5.Lcd.setTextColor(WHITE, BLACK);
    // set text size
    M5.Lcd.setTextSize(2);
    // print example name
    M5.Lcd.drawCentreString(" M5-RTC-Mod_Interrupt ", 320 / 2, 0, 2);
    M5.Lcd.setCursor(0, 30);

    // initialize I2C communication
    Wire.begin();

    // init M5-RTC-Mod
    if (rtc_mod.begin() == false) {
        M5.Lcd.println("Can't connect to M5-RTC-Mod");
        // if communication failed, loop here forever
        while (1)
            ;
    } else {
        //Updates the time variables from RTC
        if (rtc_mod.rtc.updateTime() == false) {
            Serial.print("RTC failed to update");
        } else {
            M5.Lcd.printf("Time: %02d:%02d\n", rtc_mod.rtc.getHours(), rtc_mod.rtc.getMinutes());
            M5.Lcd.printf("Date: %02d.%02d.%d\n", rtc_mod.rtc.getDate(), rtc_mod.rtc.getMonth(), rtc_mod.rtc.getYear());
        }
    }

    // set the interrupt pin as an input with internal pullup resistor
    pinMode(INT_PIN, INPUT_PULLUP);
    // interrupt is fired by a falling edge, isr is called
    attachInterrupt(INT_PIN, isr, FALLING);

    // configure the RTC to fire an periodic interrupt
    // every_second specifies the interrupt to occur either every second or every minute
    // rtc.enablePeriodicUpdateInterrupt(true);
    // dont't forget to explicit disable the Interrupt if you don't use it anymore:
    // rtc.disablePeriodicUpdateInterrupt();

    // configure the RTC to fire an interrupt on a specific date
    rtc_mod.rtc.enableAlarmInterrupt(ALM_MINUTE, ALM_HOUR, ALM_DATE_OR_WEEKDAY, ALM_IS_WEEKDAY, ALM_MODE);
    // only disables the interrupt (not the alarm flag)
    // rtc.disableAlarmInterrupt();

    // display current configuration
    M5.Lcd.setTextSize(1);
    switch (ALM_MODE) {
        case 0:
            M5.Lcd.println("Interrupt occurs when minutes, hours and weekday/date match");
            break;
        case 1:
            M5.Lcd.println("Interrupt occurs when hours and weekday/date match (once per weekday/date)");
            break;
        case 2:
            M5.Lcd.println("Interrupt occurs when minutes and weekday/date match (once per hour per weekday/date)");
            break;
        case 3:
            M5.Lcd.println("Interrupt occurs when weekday/date match (once per weekday/date)");
            break;
        case 4:
            M5.Lcd.println("Interrupt occurs when hours and minutes match (once per day)");
            break;
        case 5:
            M5.Lcd.println("Interrupt occurs when hours match (once per day)");
            break;
        case 6:
            M5.Lcd.println("Interrupt occurs when minutes match (once per hour)");
            break;
        case 7:
            M5.Lcd.println("Interrupts are all disabled – default value");
            break;
        default:
            break;
    }

    M5.Lcd.printf("Alarm set to: %02d:%02d\n", ALM_HOUR, ALM_MINUTE);
    M5.Lcd.printf("date or weekday: %d\n", ALM_DATE_OR_WEEKDAY);
    M5.Lcd.printf("weekday alarm: %d\n", ALM_IS_WEEKDAY);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("waiting for next alarm");
}

void loop() {
    // refresh display
    M5.update();

    delay(1000);
}

// the isr function is called when an interrupt occurs
void IRAM_ATTR isr() {
    // do stuff
    M5.Lcd.println("INTERRUPT");

    // interrupt was handled, flag can be deleted
    rtc_mod.rtc.clearAlarmInterruptFlag();
}
