# M5-RTC-Module

The M5-RTC-Mod is a real time clock specifically designed for the popular ESP32 based [M5-Stack](https://m5stack.com/). It features the [RV-3028-C7 RTC from Micro Crystal](https://www.microcrystal.com/en/products/real-time-clock-rtc/rv-3028-c7/).

![M5-RTC-Module upside](/extras/M5-RTC-Mod_up.jpg)
![M5-RTC-Module downside](/extras/M5-RTC-Mod_down.jpg)

## Features

+ **high precision** [RTC](https://www.microcrystal.com/en/products/real-time-clock-rtc/rv-3028-c7/) with ±1ppm deviation per year (±32 seconds per year)
+ extremely **long battery runtime** with a CR2032 lithium cell (at least 10 Years, considering the operating life of the battery)
+ fully compatible to the [M5Stack Core](https://m5stack.com/collections/m5-core)
+ two external [EEPROMs](https://www.st.com/en/memories/m24256-bw.html)
+ battery voltage monitoring through a 10-bit [ADC](http://ww1.microchip.com/downloads/en/DeviceDoc/20001805C.pdf).
+ configurable interrupt pin
+ 3D printable housing for an M5Stack
+ all components are connected via a single I2C bus

## Repository Contents

+ **/examples** – example sketches for the library
+ **/src** – source files for the library (`.cpp`, `.h`)
+ **/extras/hardware** – schematic, Gerber-files and bill of material
+ **/extras/3D-files** – `.stl`-file for 3D printing

## Dependencies

The library is based on the [RTC RV-3028-C7 Arduino Library](https://github.com/constiko/RV-3028_C7-Arduino_Library).
It also includes the [uEEPROMLib](https://github.com/Naguissa/uEEPROMLib) for communication with the external EEPROM via I2C.
The battery voltage is monitored using the [MCP3X21](https://github.com/pilotak/MCP3X21) library.

## API

The following functions are included in this library:

+ `begin()` – initializes the I2C-Interface and the RTC. Must be called in the setup function
+ `setLocalTimeToRTC()` – set the time on the ESP32 to the M5-RTC-Mod
+ `getBatteryStatus()` – returns true if the battery is ok, false if it's to low
+ `getBatteryVoltage()` – returns the battery Voltage in mV

This library works best in combination with functions from the included libraries:

+ Additional functions to control the **RTC** are described [here](https://github.com/constiko/RV-3028_C7-Arduino_Library#general-functions). You can use these by adding `.rtc` to the call of the function.
+ Additional functions to control the **EEPROM** are described [here](https://naguissa.github.io/uEEPROMLib_doc_and_extras/classuEEPROMLib.html). You can use these by adding `.eeprom0` or `.eeprom1` to the call of the function.
+ Additional functions to control the **ADC** are described [here](https://github.com/pilotak/MCP3X21). You can use these by adding `.adc` to the call of the function.

## Usage

Here is an example on how to use the library:

```cpp
#include <M5_RTC_Module.h>

// create an instance of the class M5_RTC_Mod
M5_RTC_Mod rtc_mod;

void setup(){
    // init RTC-Module
    rtc_mod.begin()
}

void loop(){
    // Updates the time variables from RTC
    if (rtc_mod.rtc.updateTime()) {
        currentTime = rtc_mod.rtc.stringTimeStamp();
    } else {
        // RTC failed to update
    }
}

```

### Choosing the interrupt pin

You can [set up an interrupt event](https://github.com/constiko/RV-3028_C7-Arduino_Library#alarm-interrupt-functions) on the RTC.
With the help of the solder jumper LJ1 on the M5-RTC-Mod board you can choose which IO of the ESP32 should act as an interrupt pin.

The following IOs can be chosen as an interrupt pin:

|  |  | | |  |  |  |
|--|--|-|-|--|--|--|
|35|36|2|5|12|13|15|

For example: In the following picture the IO 35 is used as an interrupt pin.

![LJ1](/extras/LJ1.jpg)

## Contribute

We look forward to contributions, reports or comments on the hardware or software.

If you have any question feel free to contact us: info@iotec-gmbh.de

## License

See [LICENSE](LICENSE) file for more information.

Copyright (c) 2020 iotec GmbH
