/*
 * @file M5_RTC_Module.h
 * @brief Library for using the RTC-Module from iotec for the M5-Stack
 * @author Created by Johannes W.
 * @date November 12, 2020.
 * @copyright Copyright (c) 2020 iotec GmbH
 */

#ifndef M5_RTC_Module_h
#define M5_RTC_Module_h

#include <Arduino.h>
// RTC library - see more at: https://github.com/constiko/RV-3028_C7-Arduino_Library
#include <RV-3028-C7.h>
// EEPROM library - see more at: https://github.com/Naguissa/uEEPROMLib
#include <uEEPROMLib.h>
// ADC library - see more at: https://github.com/pilotak/MCP3X21
#include <MCP3X21.h>

class M5_RTC_Mod {
   public:
    /**
     * @brief Construct a new M5-RTC-Mod object
     * 
     * @param adrEeprom0 address of eeprom 0
     * @param adrEeprom1 address of eeprom 1
     * @param adrAdc address of the ADC
     */
    M5_RTC_Mod(uint8_t adrEeprom0 = 0x50, uint8_t adrEeprom1 = 0x51, uint8_t adrAdc = 0x48);

    /**
     * @brief initialize the RTC-Module
     * 
     * @param wirePort get the I2C-interface
     * @param set24Hour select the time format you want
     * @param disableTrickleCharge enable or disable trickle charge. Disable is default.
     * For more information see: https://www.microcrystal.com/fileadmin/Media/Products/RTC/App.Manual/RV-3028-C7_App-Manual.pdf#page=48
     * @param setLevelSwitchingMode enable or disable level switching mode. Enable is default.
     * For more information see:https://www.microcrystal.com/fileadmin/Media/Products/RTC/App.Manual/RV-3028-C7_App-Manual.pdf#page=47
     *
     * @return <b>true</b> on success, <b>false</b> on error.
     */
    bool begin(TwoWire &wirePort = Wire, bool set24Hour = true, bool disableTrickleCharge = true, bool setLevelSwitchingMode = true);

    /**
     * @brief Set the local time to RTC.
     *
     * @return <b>true</b> on success, <b>false</b> on error.
     */
    bool setLocalTimeToRTC();

    /**
     * @brief Check if RTC battery is full or empty
     *
     * @return <b>true</b> when voltage > 2.5V (full), <b>false</b> when voltage <= 2.5V (empty)
     */
    bool getBatteryStatus();

    /**
     * @brief Get RTC battery voltage
     *
     * @return the current RTC battery voltage in mV
     */
    uint16_t getBatteryVoltage();

    // Adress of the ADC on the M5-RTC-Mod
    const uint8_t ADR_ADC = 0x48;

    // ADC reference voltage in mV
    const uint16_t REF_VOLTAGE = 4096;

    // Create uEEPROMLib instance;
    uEEPROMLib eeprom0;
    uEEPROMLib eeprom1;

    // Create RV3028 instance
    RV3028 rtc;

    // Create MCP3021 instance with adress of ADC
    MCP3021 adc;

   private:
    // holds the adc value
    uint16_t _valueADC;
};

#endif
