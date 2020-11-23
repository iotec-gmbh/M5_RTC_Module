/*
 * In this sketch Examples of different data types are written to the EEPROM and read out again. 
 * Then the content of the memory addresses is shown on the display.
 */

#include <M5Stack.h>
#include <Wire.h>
#include <uEEPROMLib.h>

#include "M5_RTC_Module.h"

// position where to write in eeprom
unsigned int pos = 0;

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
    M5.Lcd.drawCentreString(" M5-RTC-Mod_EEPROM ", 320 / 2, 0, 2);
    M5.Lcd.setCursor(0, 30);
    M5.Lcd.setTextSize(1);

    // init M5-RTC-Mod
    if (rtc_mod.begin()) {
        M5.Lcd.println("M5-RTC-Mod online");
    } else {
        M5.Lcd.println("Can't connect to M5-RTC-Mod");
        // if communication failed, loop here forever
        while (1)
            ;
    }

    // initialize example variables of different types
    int intTmp = 24543557;
    float floatTmp = 3.1416;
    char charTmp = 'A';
    char cString[128] = "abcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()ABCDEFGHIJKLMNOPQ";
    int stringLength = strlen(cString);

    // Write single char
    if (!rtc_mod.eeprom0.eeprom_write(8, charTmp)) {
        M5.Lcd.println("Failed to store CHAR");
    } else {
        M5.Lcd.println("CHAR correctly stored");
    }

    // Write a long string of chars FROM position 33 which isn't aligned to the 32 byte pages of the EEPROM
    if (!rtc_mod.eeprom0.eeprom_write(33, (byte *)cString, strlen(cString))) {
        M5.Lcd.println("Failed to store STRING");
    } else {
        M5.Lcd.println("STRING correctly stored");
    }

    // Write an int
    if (!rtc_mod.eeprom0.eeprom_write(0, intTmp)) {
        M5.Lcd.println("Failed to store INT");
    } else {
        M5.Lcd.println("INT correctly stored");
    }

    // write a float
    if (!rtc_mod.eeprom0.eeprom_write(4, floatTmp)) {
        M5.Lcd.println("Failed to store FLOAT");
    } else {
        M5.Lcd.println("FLOAT correctly stored");
    }

    // Flush
    intTmp = floatTmp = charTmp = 0;

    M5.Lcd.print("C string length is: ");
    M5.Lcd.println(stringLength, DEC);
    memset(cString, 0, sizeof(cString));

    M5.Lcd.print("int: ");
    rtc_mod.eeprom0.eeprom_read(0, &intTmp);
    M5.Lcd.println(intTmp);

    M5.Lcd.print("float: ");
    rtc_mod.eeprom0.eeprom_read(4, &floatTmp);
    M5.Lcd.println((float)floatTmp);

    M5.Lcd.print("char: ");
    rtc_mod.eeprom0.eeprom_read(8, &charTmp);
    M5.Lcd.println(charTmp);

    M5.Lcd.print("chararray: ");
    rtc_mod.eeprom0.eeprom_read(33, (byte *)cString, stringLength);
    M5.Lcd.println(cString);

    M5.Lcd.println();
    M5.Lcd.println("Printing value of each EEPROM address in HEX....");
}

void loop() {
    // print writing position
    M5.Lcd.print(pos);
    M5.Lcd.print(": 0x");
    // read and print eeprom value as HEX from position
    M5.Lcd.println(rtc_mod.eeprom0.eeprom_read(pos), HEX);

    // set new writing position of eeprom
    pos++;

    // refresh display
    M5.update();

    delay(500);
}
