#include "display.hpp"
#include "octopus.hpp"
#include "coffee.hpp"
#include "controller.hpp"


#define TOP_PROGRESS 16
#define BOTTOM_PROGRESS 48
#define BOTH_PROGRESS 32
#define NO_PROGRESS 0

int brew_seconds = 0;

void centerPrintToScreen(char const *str, u8g2_uint_t y)
{
  u8g2_uint_t width = u8g2.getStrWidth(str);
  u8g2.setCursor(64 / 2 - width / 2, y);
  u8g2.print(str);
}

void allignRightPrintToScreen(char const *str, u8g2_uint_t y)
{
  u8g2_uint_t width = u8g2.getStrWidth(str);
  u8g2.setCursor(100 - width, y);
  u8g2.print(str);
}

void updateDisplay()
{

  char buf[10];

  if (state == SLEEPING)
  {
    u8g2.setPowerSave(1);
  }
  else
  {
    u8g2.setPowerSave(0);
  }

  if (state == SAUBERN){

        // find out where we are in the saubern cycle
        int timeInSaubern = millis()-saubernStartedAt;
        int cycleDuration = (saubernPumpMS + saubernWaitMS);
        int saubernCycle = timeInSaubern/cycleDuration;
        int timeIncycle = (timeInSaubern - (saubernCycle * cycleDuration));
\
        if (timeIncycle - saubernPumpMS > 0){
          //waiting
          int timeToDisplay = (timeIncycle - saubernPumpMS) / 1000;

          u8g2.clearBuffer();
          u8g2.setFont(u8g2_font_fub42_tn);
          u8g2.setCursor(0, 70);
          u8g2.printf("%2i", timeToDisplay);
          u8g2.setFont(u8g2_font_fub20_tn);
          u8g2.setCursor(20, 120);
          u8g2.printf("%2i", saubernCycle);
          u8g2.sendBuffer();
        }else{
          // pumping
          int timeToDisplay = (saubernPumpMS - timeIncycle) / 1000;

          u8g2.clearBuffer();
          u8g2.setFont(u8g2_font_fub42_tn);
          u8g2.setCursor(0, 70);
          u8g2.printf("%2i", timeToDisplay);
          u8g2.setFont(u8g2_font_fub20_tn);
          u8g2.setCursor(20, 120);
          u8g2.printf("%2i", saubernCycle);
          u8g2.sendBuffer();

        }
        return;

  }
  if (state == BREWING)
  {
    Serial.print("display_brewing ");
    if (mode == OPEN_MODE)
    {
      Serial.println("- open");

      int timeToDisplay = (millis() - startedBrewAt) / 1000;

      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_fub42_tn);
      u8g2.setCursor(0, 70);
      u8g2.printf("%2i", timeToDisplay);

      u8g2.sendBuffer();
    }
    else
    {
      // timed mode
      Serial.println("- timed");
      Serial.printf("startedBrewAt: %i\n", startedBrewAt);
      Serial.printf("position: %i\n", position);

      int timeToDisplay = (millis() - startedBrewAt) / 1000;

      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_fub42_tn);
      u8g2.setCursor(0, 70);
      u8g2.printf("%2i", timeToDisplay);
      u8g2.setFont(u8g2_font_fub20_tn);
      u8g2.setCursor(20, 120);
      u8g2.printf("%2i", position);
      u8g2.sendBuffer();

      Serial.print("display done");
    }
  }
  if (state == FINISHED_BREWING)
  {
    if (millis() > finishedBrewAt + SLEEP_AFTER_MS)
    {
      u8g2.setPowerSave(1);
    }
    else
    {
      int total = (finishedBrewAt - startedBrewAt) / 1000;
      u8g2.clearBuffer();
      u8g2.drawXBM(0, 0, coffee_width, coffee_height, coffee_bits);
      u8g2.setFont(u8g2_font_fub20_tn);
      u8g2.setCursor(18, 120);
      u8g2.printf("%2i", total);
      u8g2.sendBuffer();
    }
  }
  if (state == WAITING)
  {
    if (mode == OPEN_MODE)
    {
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_fub42_tn);
      u8g2.setCursor(0, 70);
      u8g2.printf("%2i", 0);
      u8g2.sendBuffer();
    }
    else
    {
      // timed mode
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_fub42_tn);
      u8g2.setCursor(0, 70);
      u8g2.printf("%2i", position);
      u8g2.sendBuffer();
    }
  }
}

void setupDisplay()
{
  Serial.println("init_display");

  u8g2.begin();
  u8g2.enableUTF8Print(); //required for ü and such
  u8g2.clearBuffer(); // clear the internal memory
  u8g2.drawXBM(0, 0, coffeeOctopus_width, coffeeOctopus_height, coffeeOctopus_bits);
  u8g2.sendBuffer();
  Serial.println("init_display done");
  delay(3000);
}
