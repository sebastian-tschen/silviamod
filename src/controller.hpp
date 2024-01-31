
#include <Button.h>
#include <menuIO/stringIn.h>

#include <menu.h>

#define BREWING 1
#define WAITING 2
#define SLEEPING 3
#define FINISHED_BREWING 4
#define SAUBERN 5

#define TIME_MODE 1
#define OPEN_MODE 2

#define PUMP_PIN D5
#define VALVE_PIN D6
#define LED_PIN 16
#define SLEEP_AFTER_S 10
#define SLEEP_AFTER_MS 1000 * SLEEP_AFTER_S // sleep after 10 seconds

extern Button rotaryButton;
extern Button brewSwitch;
extern navRoot menuNav;
extern stringIn<0> navigationInput;

extern int position; // rotary encoder position, marking seconds of brew time

extern int state;                    // current state, can be BREWING, WAITING, SLEEPING, or FINISHED_BREWING
extern int mode;                     // current mode, can be TIME_MODE or OPEN_MODE
extern bool MENU_ON;                 // is the menu currently displayed
extern unsigned long startedBrewAt;  // millis at which the last brew started
extern unsigned long finishedBrewAt; // millis at which the last brew finished
extern unsigned long lastChange;     // when did the last user interaction happen

extern int saubernCycleCount;
extern int saubernPumpMS;
extern int saubernWaitMS;
extern unsigned int saubernStartedAt;


void control();
void initControl();
result saveState();
result startSaubern();
