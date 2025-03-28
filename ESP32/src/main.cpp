#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "secrets.env.h"
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define DHTTYPE DHT22
#define DHTPIN 5
#define PIRPIN 4
#define PRPIN 34
#define BUZZPIN 32
#define BTPIN 26

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
bool firebaseEnabled = false;  // Flag to enable/disable Firebase
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);
String uid;

void setup() {
  Serial.begin(115200);
  display.begin(0x3C, true);
  display.clearDisplay();
  display.setRotation(1);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  delayMS = 100;

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  unsigned long time = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
    //1 minute until it gives up
    if(millis() - time > 60000){
      Serial.print("Times Out connection movin on without WiFi\n");
      break;
    }
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase connected.");
    signupOK = true;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    firebaseEnabled = true;
  } else {
    Serial.printf("Firebase failed: %s\n", config.signer.signupError.message.c_str());
  }

  dht.begin();
  pinMode(PIRPIN, INPUT);
  pinMode(PRPIN, INPUT);
  pinMode(BUZZPIN, OUTPUT);
  pinMode(BTPIN, INPUT);

  uid = auth.token.uid.c_str();
}

void loop() {
  if (firebaseEnabled && Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 500 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    if (!Firebase.RTDB.setInt(&fbdo, "test/int", count)) {
      Serial.println("Firebase error: ");
      Serial.println(fbdo.errorReason());
      firebaseEnabled = false;  // Disable Firebase on failure
    }
    count++;
    if (!Firebase.RTDB.setFloat(&fbdo, "test/float", 0.01 + random(0, 100))) {
      Serial.println("Firebase error: ");
      Serial.println(fbdo.errorReason());
      firebaseEnabled = false;
    }

     display.clearDisplay();
     display.setCursor(0, 0);
     if (digitalRead(PIRPIN) == HIGH) {
       display.println("Detetado");
       Firebase.RTDB.setInt(&fbdo, "", true);
       digitalWrite(BUZZPIN, HIGH);
     } else {
       display.println("Nao Detetado");
       digitalWrite(BUZZPIN, LOW);
     }
   
     display.println(digitalRead(BTPIN) == HIGH ? "1" : "0");
     float lum = analogRead(PRPIN);
     display.printf("Luminosidade=%.2f\n", lum);
     
     sensors_event_t event;
     dht.temperature().getEvent(&event);
     display.printf("Temperatura=%.2f oC\n", event.temperature);
     dht.humidity().getEvent(&event);
     display.printf("Humidade=%.2f%%\n", event.relative_humidity);
   
     display.display();
     Serial.println(uid);
     delay(delayMS);

  }

  
}
