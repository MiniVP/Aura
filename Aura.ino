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
//#define RTC_SET
#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.println(x)
#else
 #define DEBUG_PRINT(x)
#endif

#define LED_PIN 9

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
  // The following saves some extra power by disabling some 
  // peripherals I am not using.
  
  // Disable the ADC by setting the ADEN bit (bit 7)  of the
  // ADCSRA register to zero.
  ADCSRA = ADCSRA & B01111111;
  
  // Disable the analog comparator by setting the ACD bit
  // (bit 7) of the ACSR register to one.
  ACSR = B10000000;
  
  // Disable digital input buffers on all analog input pins
  // by setting bits 0-5 of the DIDR0 register to one.
  // Of course, only do this if you are not using the analog 
  // inputs for your project.
  DIDR0 = DIDR0 | B00111111;

  pinMode(PPD_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, HIGH);
  
  Serial.begin(115200);
  
  DEBUG_PRINT("Starting...");

  #ifdef SD_OK
    if (!SD.begin(SD_CS_PIN)) {
      DEBUG_PRINT(F("SD initialisation failed"));
      blink(4);
    } else {
      DEBUG_PRINT("SD init OK");
      dataFile = SD.open(F("data.csv"), FILE_WRITE);
      if (!dataFile) {
        DEBUG_PRINT(F("File opening failed"));
        blink(8);
      }
      DEBUG_PRINT("SD File data.csv opened OK");
    }
  #endif

  #ifdef RTC_OK
    rtc.begin();
    #ifdef RTC_SET
      rtc.setDOW(SATURDAY);
      rtc.setTime(20, 41, 0);
      rtc.setDate(23, 8, 2018);
    #endif
    DEBUG_PRINT("RTC init OK");
    DEBUG_PRINT(rtc.getDateStr());
    DEBUG_PRINT(rtc.getTimeStr());
    //DEBUG_PRINT(readVccAtmega());
  #endif

  digitalWrite(LED_PIN, LOW);

  DEBUG_PRINT("Ready ! Data printed each 30 seconds");
  
  starttime = millis();
}

void loop() {
  duration = pulseIn(PPD_PIN, LOW);
  lowpulseoccupancy = lowpulseoccupancy + duration;

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
    //output.concat(readVccAtmega());
    //output.concat(',');
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

long readTempAtmega() { 
  long result; // Read temperature sensor against 1.1V reference 
  ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3); 
  delay(2); // Wait for Vref to settle 
  ADCSRA |= _BV(ADSC); // Convert 
  while (bit_is_set(ADCSRA,ADSC)); 
  result = ADCL; 
  result |= ADCH<<8; 
  result = (result - 125) * 1075; 
  return result;
}

long readVccAtmega() { 
  long result; // Read 1.1V reference against AVcc 
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); 
  delay(2); // Wait for Vref to settle 
  ADCSRA |= _BV(ADSC); // Convert 
  while (bit_is_set(ADCSRA,ADSC)); 
  result = ADCL; 
  result |= ADCH<<8; 
  result = 1126400L / result; // Back-calculate AVcc in mV 
  return result; 
}

void blink(byte nb) { 
  while (true) {
    for (byte i = 0; i < nb; i++) {
      digitalWrite(LED_PIN, LOW);
      delay(250);
      digitalWrite(LED_PIN, HIGH);
      delay(250);
    }

    delay(1500);
  }
}

