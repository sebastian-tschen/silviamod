#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Button.h>

#include "display.hpp"
#include "controller.hpp"
#include "rotary.hpp"

#define MENU_ASYNC

#include <menu.h>
#include <menuIO/u8g2Out.h>
// #include <menuIO/encoderIn.h>
// #include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/stringIn.h>


using namespace Menu;

#define fontName u8g2_font_7x13_mf
#define fontX 7
#define fontY 16
#define offsetX 0
#define offsetY 3
#define U8_Width 64
#define U8_Height 128

#define ENC_A D7
#define ENC_B 10
#define BUTTON D3

#define BREW_SWITCH 3


int last = 0;
const colorDef<uint8_t> colors[6] MEMMODE={
  {{0,0},{0,1,1}},//bgColor
  {{1,1},{1,0,0}},//fgColor
  {{1,1},{1,0,0}},//valColor
  {{1,1},{1,0,0}},//unitColor
  {{0,1},{0,0,1}},//cursorColor
  {{1,1},{1,0,0}},//titleColor
};

MENU(mainMenu,"Hauptmenü",doNothing,noEvent,wrapStyle
  ,OP("Saubern",startSaubern,enterEvent)
  ,OP("Speichern",saveState,enterEvent)
  ,EXIT("<zurück")
);

#define MAX_DEPTH 2

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R1);

MENU_OUTPUTS(out,MAX_DEPTH
  ,U8G2_OUT(u8g2,colors,fontX,fontY,offsetX,offsetY,{0,0,U8_Width/fontX,U8_Height/fontY})
  ,SERIAL_OUT(Serial)
);


stringIn<0> navigationInput;//buffer size: 2^5 = 32 bytes, eventually use 0 for a single byte
NAVROOT(menuNav,mainMenu,MAX_DEPTH,navigationInput,out);

Button rotaryButton(BUTTON);
Button brewSwitch(BREW_SWITCH);

bool MENU_ON = false;

//idle method is used to set MENU_ON
result idle(menuOut& o,idleEvent e) {
  o.clear();
  switch(e) {
    case idleStart:MENU_ON=false;Serial.println("idleStart");break;
    case idling:MENU_ON=false;Serial.println("idling");break;
    case idleEnd:Serial.println("idleEnd");break;
  }
  return proceed;
}

void setup()
{
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);
  pinMode(LED_PIN, INPUT);
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

  menuNav.idleTask=idle;
  menuNav.timeOut = SLEEP_AFTER_S;
}

void loop()
{
  control();
  if (MENU_ON){
    u8g2.setFont(fontName);
    menuNav.doInput();
    if (menuNav.changed(0)) {//only draw if menu changed for gfx device
      //change checking leaves more time for other tasks
      u8g2.firstPage();
      do menuNav.doOutput(); while(u8g2.nextPage());
    }
  }else{
    // if we are not in the Menu, display is drawn by the updateDisplay method
    updateDisplay();
  }
}
