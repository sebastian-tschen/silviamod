#pragma once

#include <SPI.h>
#include <U8g2lib.h>
#include "controller.hpp"

extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
void setupDisplay();
void updateDisplay();
