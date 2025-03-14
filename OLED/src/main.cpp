#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_SH110X.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

// OLED FeatherWing buttons map to different pins depending on board:
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
#elif defined(ESP32) && \
    !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && \
    !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3) && \
    !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3_NOPSRAM)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
#elif defined(ARDUINO_NRF52832_FEATHER)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
#else // 32u4, M0, M4, nrf52840, esp32-s2, esp32-s3 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
#endif

//Pins Sensores
#define DHTTYPE    DHT22
#define DHTPIN 5     // Digital pin connected to the DHT sensor 
#define PIRPIN 4     // Digital pin connected to the PIR sensor 
#define PRPIN 34     // Digital pin connected to the PR sensor 
#define BUZZPIN 32     // Digital pin connected to the Buzzer 

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

void setup() {
  Serial.begin(115200);

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);

  pinMode(PIRPIN,INPUT);
  pinMode(PRPIN,INPUT);
  pinMode(BUZZPIN,OUTPUT);
  
  Serial.println("128x64 OLED FeatherWing test");
  delay(250); // wait for the OLED to power up
  display.begin(0x3C, true); // Address 0x3C default

  Serial.println("OLED begun");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setRotation(1);
  Serial.println("Button test");

  // text display tests
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.display(); // actually display all of the above

  delayMS = sensor.min_delay / 1000;

}

void loop() {
  //Sensor PIR
  if (digitalRead(PIRPIN) == HIGH){
    display.println("OLa");
    digitalWrite(BUZZPIN, HIGH);
  } 
  else {
    display.println("Adeus");
    digitalWrite(BUZZPIN, LOW);
  }

  //Sensor PR
  display.println(analogRead(PRPIN));

  //Sensor
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  display.println(event.temperature); 
  dht.humidity().getEvent(&event);
  display.println(event.relative_humidity);
  delay(delayMS);
  yield();
  display.display();
  delay(10);
  display.clearDisplay();
  display.setCursor(0,0);
} 