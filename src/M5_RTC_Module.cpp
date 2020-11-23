/*
 * @file M5_RTC_Module.cpp
 * @brief Library for using the RTC-Module from iotec for the M5-Stack
 * @author Created by Johannes W.
 * @date November 12, 2020.
 * @copyright Copyright (c) 2020 iotec GmbH
 */

#include <Arduino.h>
#include <M5_RTC_Module.h>

M5_RTC_Mod::M5_RTC_Mod(uint8_t adrEeprom0, uint8_t adrEeprom1, uint8_t adrAdc)
    : eeprom0(adrEeprom0),
      eeprom1(adrEeprom1),
      rtc(),
      adc(adrAdc) {
}

bool M5_RTC_Mod::begin(TwoWire &wirePort, bool set24Hour, bool disableTrickleCharge, bool setLevelSwitchingMode) {
    // initialize I2C communication
    wirePort.begin();

    // initialize adc
    adc.init(&wirePort);

    // initialize real time clock
    if (rtc.begin(wirePort, set24Hour, disableTrickleCharge, setLevelSwitchingMode)) {
        return true;
    } else {
        return false;
    }

    // set all values in the rtc to default and clear all flags
    rtc.clearInterrupts();
    rtc.disableAlarmInterrupt();
    rtc.clearAlarmInterruptFlag();
    rtc.disableTimer();
    rtc.disableTimerInterrupt();
    rtc.clearTimerInterruptFlag();
    rtc.disablePeriodicUpdateInterrupt();
    rtc.clearPeriodicUpdateInterruptFlag();
    rtc.disableClockOut();
    rtc.clearClockOutputInterruptFlag();
    rtc.reset();

    // write system time to RTC
    setLocalTimeToRTC();
}

// write time to RTC
bool M5_RTC_Mod::setLocalTimeToRTC() {
    // store time struct
    struct tm timeinfo;
    // read system time
    if (!getLocalTime(&timeinfo)) {
        return false;
    }
    // set time to rtc
    if (rtc.setTime(timeinfo.tm_sec, timeinfo.tm_min, timeinfo.tm_hour, timeinfo.tm_wday,
                    timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900) == false) {
        return false;
    }
    return true;
}

uint16_t M5_RTC_Mod::getBatteryVoltage() {
    // get value from adc
    _valueADC = adc.read();
    // return calculated voltage in mV
    return adc.toVoltage(_valueADC, REF_VOLTAGE);
}

bool M5_RTC_Mod::getBatteryStatus() {
    // get voltage from adc
    if (getBatteryVoltage() > 2500) {
        // battery voltage ok
        return true;
    } else {
        // battery voltage too low
        return false;
    }
}
