/*
 * Get the time over a NTP-Server and set the RV3028 Real Time Clock
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

// Create M5_RTC_Mod instance
M5_RTC_Mod rtc_mod;

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
    M5.Lcd.drawCentreString(" M5-RTC-Mod_NTP-Sync ", 320 / 2, 0, 2);
    M5.Lcd.setCursor(0, 30);
    M5.Lcd.setTextSize(1);

    // initialize I2C communication
    Wire.begin();

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
    if (rtc_mod.setLocalTimeToRTC()) {
        M5.Lcd.println("RTC set correctly");
    } else {
        M5.Lcd.println("Something went wrong setting the time");
    }

    // disconnect WiFi as it's no longer needed
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    M5.Lcd.setTextSize(2);
}

void loop() {
    // updates the time variables from RTC and print out
    if (rtc_mod.rtc.updateTime() == false) {
        M5.Lcd.print("RTC failed to update");
    } else {
        String currentTime = rtc_mod.rtc.stringTimeStamp();
        M5.Lcd.setCursor(0, 60);
        M5.Lcd.print(currentTime);
    }

    // refresh display
    M5.update();

    delay(1000);
}
