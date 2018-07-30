// Pour le senseur de température
#include <OneWire.h>
#include <DallasTemperature.h>

// Pour l'écran OLED
#include "U8glib.h"

#define ONE_WIRE_BUS 8
#define UP_PIN 2
#define DOWN_PIN 3

const int debounceTime = 500;

const int temperatureDelay = 5; // Temps en secondes
const float maxTemp = 16.0;
const float minTemp = 4.0;
const float tempInterval = 0.5;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

U8GLIB_SSD1306_128X64 display(U8G_I2C_OPT_NONE);

float desired_temperature = 6.0;

void setup(void) {
  Serial.begin(9600);

  sensors.begin();
  attachInterrupt(digitalPinToInterrupt(UP_PIN), tempUp, RISING);
  attachInterrupt(digitalPinToInterrupt(DOWN_PIN), tempDown, RISING);
}

void loop(void) {
  display.firstPage();

  do {
    display.setFont(u8g_font_gdr14r);
    display.drawStr(0, 16, dtostrf(desired_temperature, 6, 1, "     "));

    display.setFont(u8g_font_gdb30r);
    display.drawStr(0, 55, dtostrf(getTemperature(), 5, 2, "    "));

  } while ( display.nextPage() );
}

void tempUp() {
  static long last_millis;

  if (millis() - last_millis > debounceTime) {
    desired_temperature += tempInterval;
    if (desired_temperature > maxTemp) {
      desired_temperature = maxTemp;
    }
    last_millis = millis();
  }
}

void tempDown() {
  static long last_millis;

  if (millis() - last_millis > debounceTime) {
    desired_temperature -= tempInterval;
    if (desired_temperature < minTemp) {
      desired_temperature = minTemp;
    }
    last_millis = millis();
  }
}

static inline float getTemperature() {
  static unsigned long lastMillis;
  static float temperature = 0;

  unsigned long now = millis();
  if (now - lastMillis >= temperatureDelay * 1000) {
    lastMillis = now;
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);
  }
  return temperature;
}
