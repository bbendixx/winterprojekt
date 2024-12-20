#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define LED_PIN D1
#define BUTTON_PIN D2
#define STRIPSIZE 12

const char *ssid = "SSID!";
const char *password = "PASSWORD:(";
const long utcOffsetInSeconds = 3600;

char daysOfTheWeek[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIPSIZE, LED_PIN, NEO_GRB + NEO_KHZ800);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

int currentCycle = 1;
unsigned long startTime = 0;
bool timerRunning = false;
unsigned long lastUpdate = 0;
bool breakTime = false;
bool timerComplete = false;

void setup() {
  delay(5000); // Ohne wollte es nicht
  Serial.begin(9600);
  Serial.println("HALLO");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.setBrightness(50);
  strip.show();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Ich verbinde mich mit dem Internet...");
  }

  Serial.println("Ich bin mit dem Internet verbunden!");
  timeClient.begin();
}

void loop() {
  timeClient.update();

  if (!digitalRead(BUTTON_PIN)) {
    if (!timerRunning && !timerComplete) {
      startTimer();
    }
  }

  if (timerRunning) {
    unsigned long elapsedTime = millis() - startTime;
    int secondsLeft = (1440000 - elapsedTime) / 1000;

    if (elapsedTime - lastUpdate >= 120000) {
      lastUpdate = elapsedTime;
      turnOnLamp();
    }

    if (elapsedTime >= 1440000) {
      currentCycle++;
      if (currentCycle > 4) {
        endTimer();
        timerComplete = true;
      } else {
        breakTime();
      }
      timerRunning = false;
      breakTime = true;
    }
  }

  if (breakTime) {
    unsigned long elapsedBreakTime = millis() - startTime;
    if (elapsedBreakTime >= 300000) {
      breakTime = false;
      if (!timerComplete) {
        startTimer();
      }
    }
  }
}

void startTimer() {
  timerRunning = true;
  startTime = millis();
  strip.fill(strip.Color(0, 0, 0));
  strip.show();
  Serial.println("Ran an die Arbeit!");
  setCycleColor();
}

void breakTime() {
  Serial.println("Pausezeit! Aber dalli!");
  strip.fill(strip.Color(0, 0, 0));
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(255, 255, 255));
  }
  strip.show();
  delay(300000);
}

void endTimer() {
  Serial.println("Geschafft! Yippie!");
  strip.fill(strip.Color(0, 0, 0));
  strip.show();
  delay(5000);
  strip.fill(strip.Color(0, 0, 0));
  strip.show();
}

void turnOnLamp() {
  int lampsToTurnOn = (millis() - startTime) / 120000;
  for (int i = 0; i < lampsToTurnOn; i++) {
    if (currentCycle == 1) {
      strip.setPixelColor(i, strip.Color(255, 0, 0));
    } else if (currentCycle == 2) {
      strip.setPixelColor(i, strip.Color(255, 165, 0));
    } else if (currentCycle == 3) {
      strip.setPixelColor(i, strip.Color(0, 255, 0));
    } else if (currentCycle == 4) {
      strip.setPixelColor(i, strip.Color(0, 125, 0));
    }
  }
  strip.show();
}

void setCycleColor() {
  strip.fill(strip.Color(0, 0, 0));
  strip.show();
}
