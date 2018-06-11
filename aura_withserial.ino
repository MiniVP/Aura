/*
 MiniVP Aura v0.2
 PPD 1 => GND
 PPD 3 => 5VDC
 PPD 4 => D8
 SD 3V3  => 3.3V
 SD GND  => GND
 SD CS   => D4
 SD MOSI => D11
 SD MISO => D12
 SD CLK  => D13
 RTC VCC => 5V or 3.3V
 RTC GND => GND
 RTC SDA => SDA
 RTC SCL => SCL
 */

#include <SPI.h>
#include <SD.h>
#include <DS3231.h>

File dataFile;
DS3231 rtc(SDA, SCL);

const int PPD_PIN = 8;
const int SD_CS_PIN = 4;
unsigned long duration;
unsigned long starttime;
const unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
String output = "";

void setup() {
  pinMode(PPD_PIN, INPUT);
  Serial.begin(115200);
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("SD initialisation failed"));
    while (1);
  }
  dataFile = SD.open(F("data.csv"), FILE_WRITE);
  if (!dataFile) {
    Serial.println(F("File opening failed"));
    while (1);
  }
  rtc.begin();
  rtc.setDOW(MONDAY);
  rtc.setTime(22, 0, 0);
  rtc.setDate(11, 6, 2018);
  starttime = millis();
}

void loop() {
  duration = pulseIn(PPD_PIN, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;

  if ((millis()-starttime) > sampletime_ms)
  {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    output.concat(rtc.getDateStr());
    output.concat(',');
    output.concat(rtc.getTimeStr());
    output.concat(',');
    output.concat(rtc.getTemp());
    output.concat(',');
    output.concat(lowpulseoccupancy);
    output.concat(',');
    output.concat(ratio);
    output.concat(',');
    output.concat(concentration);
    Serial.println(output);
    dataFile.println(output);
    dataFile.flush();
    lowpulseoccupancy = 0;
    output = "";
    starttime = millis();
  }
}

