#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Button.h>

#include "display.hpp"
#include "controller.hpp"
#include "rotary.hpp"

#define LED_PIN 16

#define ENC_A D7
#define ENC_B 10
#define BUTTON D3

#define BREW_SWITCH 3

Button rotaryButton(BUTTON);
Button brewSwitch(BREW_SWITCH);

void setup()
{
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, 1);
  digitalWrite(VALVE_PIN, 1);

  Serial.begin(115200);
  setupDisplay();
  setupRotary(ENC_A, ENC_B);
  rotaryButton.begin();
  brewSwitch.begin();
  initControl();

  WiFi.persistent(false); // disable saving wifi config into SDK flash area
  WiFi.forceSleepBegin(); // disable AP & station by calling "WiFi.mode(WIFI_OFF)" & put modem to sleep
}

void loop()
{
  static int lastCounter = 0;
  // position = counter;
  updateDisplay();
  control();
}
