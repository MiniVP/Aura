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

#define SD_OK
#define RTC_OK
#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.println(x)
#else
 #define DEBUG_PRINT(x)
#endif

#ifdef SD_OK
  #include <SPI.h>
  #include <SD.h>
  
  #define SD_CS_PIN 4
  
  File dataFile;
#endif

#ifdef RTC_OK
  #include <DS3231.h>
  
  DS3231 rtc(SDA, SCL);
#endif

#define PPD_PIN 8

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

  DEBUG_PRINT("Starting...");

  #ifdef SD_OK
    if (!SD.begin(SD_CS_PIN)) {
      DEBUG_PRINT(F("SD initialisation failed"));
    } else {
      DEBUG_PRINT("SD init OK");
      dataFile = SD.open(F("data.csv"), FILE_WRITE);
      if (!dataFile) {
        DEBUG_PRINT(F("File opening failed"));
      }
      DEBUG_PRINT("SD File data.csv opened OK");
    }
  #endif

  #ifdef RTC_OK
    rtc.begin();
    rtc.setDOW(MONDAY);
    rtc.setTime(10, 50, 0);
    rtc.setDate(18, 8, 2018);
    DEBUG_PRINT("RTC init OK");
    DEBUG_PRINT(rtc.getDateStr());
    DEBUG_PRINT(rtc.getTimeStr());
  #endif
  
  starttime = millis();

  DEBUG_PRINT("Ready ! Data printed each 30 seconds");
}

void loop() {
  duration = pulseIn(PPD_PIN, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;

  if ((millis()-starttime) > sampletime_ms)
  {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    #ifdef RTC_OK
      output.concat(rtc.getDateStr());
      output.concat(',');
      output.concat(rtc.getTimeStr());
      output.concat(',');
      output.concat(rtc.getTemp());
      output.concat(',');
    #endif
    output.concat(lowpulseoccupancy);
    output.concat(',');
    output.concat(ratio);
    output.concat(',');
    output.concat(concentration);
    
    DEBUG_PRINT(output);

    #ifdef SD_OK
      if (dataFile) {
        dataFile.println(output);
        dataFile.flush();
      }
    #endif
    
    lowpulseoccupancy = 0;
    output = "";
    starttime = millis();
  }
}

