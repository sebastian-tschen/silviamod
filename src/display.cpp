#include "display.hpp"
#include "octopus.hpp"
#include "coffee.hpp"
#include "controller.hpp"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

#define SLEEP_AFTER_MS 10 * 1000 // sleep after 10 seconds
#define TOP_PROGRESS 16
#define BOTTOM_PROGRESS 48
#define BOTH_PROGRESS 32
#define NO_PROGRESS 0

int brew_seconds = 0;

void centerPrintToScreen(char const *str, u8g2_uint_t y)
{
  u8g2_uint_t width = u8g2.getStrWidth(str);
  u8g2.setCursor(128 / 2 - width / 2, y);
  u8g2.print(str);
}

void allignRightPrintToScreen(char const *str, u8g2_uint_t y)
{
  u8g2_uint_t width = u8g2.getStrWidth(str);
  u8g2.setCursor(100 - width, y);
  u8g2.print(str);
}

void displayFullScreenProgress(char const *content, double progress, const uint8_t *font, int fontHeight)
{

  u8g2.clearBuffer();

  int rad = (int)((progress) * 64.0);
  if (rad > 0)
  {
    u8g2.setDrawColor(1);
    u8g2.drawDisc(64, BOTH_PROGRESS, rad, U8G2_DRAW_ALL);
  }

  u8g2.setDrawColor(2);
  u8g2.setFontMode(1);
  u8g2.setFont(font);
  centerPrintToScreen(content, 32 + (fontHeight / 2));

  u8g2.sendBuffer();
}

void displayTopBottomProgress(char const *top, char const *bottom, double progress, int progressType, const uint8_t *topFont, const uint8_t *bottomFont)
{

  u8g2.clearBuffer();

  if (progressType != 0)
  {
    int rad = (int)((progress) * 64.0);
    if (rad < 0)
    {
      rad = 0;
    }
    u8g2.setDrawColor(1);
    u8g2.drawDisc(64, progressType, rad, U8G2_DRAW_ALL);
    if (progressType == TOP_PROGRESS)
    {
      u8g2.setDrawColor(0);
      u8g2.drawBox(0, 32, 128, 32);
    }
    if (progressType == BOTTOM_PROGRESS)
    {
      u8g2.setDrawColor(0);
      u8g2.drawBox(0, 0, 128, 32);
    }
  }

  u8g2.setDrawColor(2);
  u8g2.setFontMode(1);
  u8g2.setFont(topFont);
  allignRightPrintToScreen(top, 27);
  u8g2.setFont(bottomFont);
  allignRightPrintToScreen(bottom, 58);

  u8g2.sendBuffer();
}
void displayTopBottomProgress(char const *top, char const *bottom, double progress, int progressType)
{
  displayTopBottomProgress(top, bottom, progress, progressType, u8g2_font_luRS19_tf, u8g2_font_luRS19_tf);
}

void updateDisplay()
{

  char buf[10];

  if (state == SLEEPING)
  {
    u8g2.setPowerSave(1);
  }else{
    u8g2.setPowerSave(0);
  }

  if (state == BREWING)
  {
    Serial.print("display_brewing ");
    if (mode == OPEN_MODE)
    {
      Serial.println("- open");

      int timeToDisplay = (millis() - startedBrewAt + 1000) / 1000;

      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_fub49_tn);
      u8g2.setCursor(3, 61);
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
      u8g2.setFont(u8g2_font_fub49_tn);
      u8g2.setCursor(3, 61);
      u8g2.printf("%2i", timeToDisplay);
      u8g2.setFont(u8g2_font_fub20_tn);
      u8g2.setCursor(90, 25);
      u8g2.printf("%2i", position);
      u8g2.sendBuffer();

      Serial.print("display done");
    }
  }
  if (state == FINISHED_BREWING)
  {
    u8g2.clearBuffer();
    u8g2.drawXBM(0, 0, coffee_width, coffee_height, coffee_bits);
    u8g2.sendBuffer();
  }
  if (state == WAITING)
  {
    if (mode == OPEN_MODE)
    {

      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_fub49_tn);
      centerPrintToScreen("0", 61);
      u8g2.sendBuffer();

    }
    else
    {
      // timed mode
      snprintf(buf, sizeof(buf), "%i", position);
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_fub49_tn);
      centerPrintToScreen(buf, 61);
      u8g2.sendBuffer();
    }
  }
}

void setupDisplay()
{
  Serial.println("init_display");

  u8g2.begin();
  u8g2.clearBuffer(); // clear the internal memory
  u8g2.drawXBM(0, 0, coffeeOctopus_width, coffeeOctopus_height, coffeeOctopus_bits);
  u8g2.sendBuffer();
  Serial.println("init_display done");
  delay(1000);
}
