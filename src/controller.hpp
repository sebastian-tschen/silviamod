
#include <Button.h>

#define BREWING 1
#define WAITING 2
#define SLEEPING 3
#define FINISHED_BREWING 4
#define TIME_MODE 4
#define OPEN_MODE 5

#define PUMP_PIN D5
#define VALVE_PIN D6

extern Button rotaryButton;
extern Button brewSwitch;

extern int position; // rotary encoder position, marking seconds of brew time

extern int state;                    // current state, can be BREWING, WAITING, SLEEPING, or FINISHED_BREWING
extern int mode;                     // current mode, can be TIME_MODE or OPEN_MODE
extern unsigned long startedBrewAt;  // millis at which the last brew started
extern unsigned long finishedBrewAt; // millis at which the last brew finished
extern unsigned long lastChange;     // when did the last user interaction happen

void control();
void initControl();
