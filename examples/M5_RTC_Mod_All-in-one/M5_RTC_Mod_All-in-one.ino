/*
 * In this sketch the RTC is synchronized witch an NTP-server.
 * Once in an hour the voltage of the battery is read out.
 * The values ​​are stored in the EEPROM together with a timestamp.
 * If the battery of the RTC is to low a message is printed
 * and the program will loop until restart.
 */

#include <M5Stack.h>
#include <WiFi.h>
#include <time.h>

#include "M5_RTC_Module.h"

const char *SSID = "YOUR_SSID";
const char *PASSWORD = "YOUR_PASS";

// select your local NTP server from this site: https://www.ntppool.org/zone/@
const char *NTP_SERVER = "pool.ntp.org";
// select your timezone from this list: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
const char *TIME_ZONE = "CET-1CEST,M3.5.0/02,M10.5.0/03";

// The below variables control what the alarm will be set to
const uint8_t ALM_MINUTE = 12;
const uint8_t ALM_HOUR = 10;
const uint16_t ALM_DATE_OR_WEEKDAY = 2;
const bool ALM_IS_WEEKDAY = false;
// Alarm when minutes match
// More informations can be found here: https://github.com/constiko/RV-3028_C7-Arduino_Library
const uint8_t ALM_MODE = 6;

// Create M5_RTC_Mod instance
M5_RTC_Mod rtc_mod;

// position where to write in EEPROM
unsigned int pos;

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
    M5.Lcd.drawCentreString(" M5-RTC-Mod_All-in-one ", 320 / 2, 0, 2);
    M5.Lcd.setCursor(0, 30);

    // init M5-RTC-Module
    if (rtc_mod.begin()) {
        M5.Lcd.println("M5-RTC-Mod online");
    } else {
        M5.Lcd.println("Can't connect to M5-RTC-Mod");
        // if communication failed, loop here forever
        while (1)
            ;
    }

    // connect to WiFi
    M5.Lcd.setTextSize(1);
    M5.Lcd.printf("Connecting to %s ", SSID);
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.print(".");
    }
    M5.Lcd.println(" CONNECTED");

    // init and get the local time
    configTzTime(TIME_ZONE, NTP_SERVER);

    // write time to RTC
    M5.Lcd.setTextSize(2);
    if (rtc_mod.setLocalTimeToRTC()) {
        // RTC set correctly, print it's time
        M5.Lcd.printf("Time: %02d:%02d\n", rtc_mod.rtc.getHours(), rtc_mod.rtc.getMinutes());
        M5.Lcd.printf("Date: %02d.%02d.%d\n", rtc_mod.rtc.getDate(), rtc_mod.rtc.getMonth(), rtc_mod.rtc.getYear());
    } else {
        M5.Lcd.println("Something went wrong setting the time");
    }

    // disconnect WiFi as it's no longer needed
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    // Enable alarm interrupt
    rtc_mod.rtc.enableAlarmInterrupt(ALM_MINUTE, ALM_HOUR, ALM_DATE_OR_WEEKDAY, ALM_IS_WEEKDAY, ALM_MODE);
    // Only disables the interrupt (not the alarm flag)
    rtc_mod.rtc.disableAlarmInterrupt();

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

    M5.Lcd.printf("Alarm set to: %02d:%02d \n", ALM_HOUR, ALM_MINUTE);

    M5.Lcd.printf("date or weekday: %d\n", ALM_DATE_OR_WEEKDAY);
    M5.Lcd.printf("weekday alarm: %d \n", ALM_IS_WEEKDAY);

    M5.Lcd.setTextSize(2);
    M5.Lcd.println("waiting for next alarm");
}

void loop() {
    // check if RTC battery is ok
    if (rtc_mod.getBatteryStatus()) {
        // String where the time is stored
        String currentTime = " ";

        // Updates the time variables from RTC
        if (rtc_mod.rtc.updateTime()) {
            currentTime = rtc_mod.rtc.stringTimeStamp();
        } else {
            M5.Lcd.print("RTC failed to update");
        }

        // poll alarm interrupt flag
        if (rtc_mod.rtc.readAlarmInterruptFlag()) {
            // read adc value
            uint16_t valueADC = rtc_mod.adc.read();
            // calculate voltage
            float volt = rtc_mod.adc.toVoltage(valueADC, rtc_mod.REF_VOLTAGE) / 1000.0;
            // parse float to string
            String voltStr = String(volt);

            // merge voltage and timestamp in one string
            String timestamp = voltStr + "V " + currentTime;
            // save string in char array to write into eeprom
            char timestampChar[timestamp.length() + 1] = {0};
            timestamp.toCharArray(timestampChar, sizeof(timestampChar));

            // write the char array into eeprom
            if (!rtc_mod.eeprom0.eeprom_write(pos, (byte *)timestampChar, sizeof(timestampChar))) {
                M5.Lcd.println("Failed to store timestamp in EEPROM");
            } else {
                M5.Lcd.println("The timestamp has been stored in EEPROM.");
            }

            // read eeprom to verrify data
            char storedTimestamp[sizeof(timestampChar)] = {0};
            rtc_mod.eeprom0.eeprom_read(pos, (byte *)storedTimestamp, sizeof(storedTimestamp));

            M5.Lcd.println(storedTimestamp);

            // set new writing position of eeprom
            pos += sizeof(timestampChar);

            // clear RTC interrupt flag
            rtc_mod.rtc.clearAlarmInterruptFlag();

            M5.Lcd.println("waiting for next alarm");
        }
    } else {
        // Battery is empty. Print a message with the current voltage
        M5.Lcd.printf("LOW BATTERY: %.2fV \n", rtc_mod.getBatteryVoltage() / 1000.0);
        // loop here forever
        while (1)
            ;
    }

    // refresh display
    M5.update();

    delay(1000);
}
