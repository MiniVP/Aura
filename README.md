# Aura

First MiniVP weather station prototype with a PPD42NS particulate matter sensor. In roman and greek mythology, Aura is a minor deity whose name means breeze.

## Components

* Arduino (tested with Uno R3)
* microSD card module
* DS3231 real time clock module
* Shinyei PPD42NS "Dust Sensor"
* Optional : Solar panel, battery and battery charger circuits

## Goals

* Design a working prototype of an autonomous station with solar charging
* Optimize energy consumption
* Measure PM2.5 or PM1
* Reach at least 24 hours of battery life with one measurement log every 5 minutes

## Pinout

Arduino Uno | Components
----------- | ----------
5V          | PPD42NS port 3
3.3V        | SD 3V3, RTC VCC
GND         | PPD42NS port 1, RTC GND, SD GND, LED GND
SDA         | RTC SDA
SCL         | RTC SCL
D4          | SD CS
D8          | PPD42NS port 4
D9          | LED
D11         | SD MOSI
D12         | SD MISO
D13         | SD CLK

## Setup

1. Clone or download this repository.
2. Move the contents of the `libraries/` folder into your Arduino IDE libraries folder.
3. Open `aura.ino` in the Arduino IDE. You can use `aura_withserial.ino` if you want to output to serial as well.
4. Edit the `PPD_PIN` and `RTC_CS_PIN` constants if you need to move those pins.
5. Edit the `rtc.setDOW`, `rtc.setTime`, `rtc.setDate` instructions to fit your current date and time.
6. Load the firmware onto the Arduino.
7. Once the RTC clock has been set once, you can comment the previously edited `rtc.set` instructions to prevent a clock reset at each Arduino reboot.
8. Resend the firmware to the Arduino.
