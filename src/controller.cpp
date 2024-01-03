#include "controller.hpp"
#include "rotary.hpp"
#include <Arduino.h>
#include <Button.h>
#include <EEPROM.h>

#define EEPROM_SIZE 50

int position = 30; // rotary encoder position, marking seconds of brew time
unsigned long lastChange;
int state = WAITING;
int mode = TIME_MODE;
unsigned long startedBrewAt = 0;
unsigned long finishedBrewAt = 0;

void saveState()
{
    int address = 0;
    int savedMode;
    int savedPosition;
    EEPROM.begin(EEPROM_SIZE);

    EEPROM.get(address, savedMode);
    address += sizeof(mode);
    EEPROM.get(address, savedPosition);

    if (savedPosition == position && savedMode == mode)
    {
        Serial.printf("no save nessecary\n");
    }
    else
    {
        int address = 0;
        EEPROM.put(address, mode);
        address += sizeof(mode);
        EEPROM.put(address, position);
        address += sizeof(position);
        EEPROM.commit();
        Serial.printf("written %i bytes to flash\n", address);
    }
    EEPROM.end();
}

void loadState()
{
    int address = 0;
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(address, mode);
    address += sizeof(mode);
    EEPROM.get(address, position);
    address += sizeof(position);
    EEPROM.end();
    Serial.printf("read %i bytes from flash", address);

    if (mode != TIME_MODE && mode != OPEN_MODE)
    {
        mode = OPEN_MODE;
    }
    position = setPosition(position);
}

void switchMode()
{

    if (mode == OPEN_MODE)
    {
        mode = TIME_MODE;
    }
    else if (mode == TIME_MODE)
    {
        mode = OPEN_MODE;
    }
    lastChange = millis();
}

bool timedBrewFinished()
{
    if (mode == OPEN_MODE)
    {
        return false;
    }
    return millis() > (startedBrewAt + (position * 1000l));
}

void startBrew()
{
    Serial.println("starting Brew");
    digitalWrite(VALVE_PIN, 0);
    digitalWrite(PUMP_PIN, 0);
    startedBrewAt = millis();
    digitalWrite(LED_PIN,1);
    state = BREWING;
    Serial.println(state);
}

void stopBrew()
{
    Serial.println("stopping Brew");
    digitalWrite(VALVE_PIN, 1);
    digitalWrite(PUMP_PIN, 1);
    finishedBrewAt = millis();
    lastChange = millis();
    brewSwitch.has_changed(); // reset has_changed_state of switch to detect reenabling;
    state = FINISHED_BREWING;
    digitalWrite(LED_PIN,0);
    Serial.println(state);
}

void initControl()
{
    loadState();
    if (brewSwitch.read() == brewSwitch.PRESSED)
    {
        mode = TIME_MODE;
        state = FINISHED_BREWING;
    }
}

void control()
{
    if (position != getPosition())
    {
        if (mode == TIME_MODE){
            position = getPosition();
        }else{
            // don't allow change of time during open mode
            setPosition(position);
        }
        lastChange = millis();
    }

    if (rotaryButton.pressed())
    {
        switchMode();
        return;
    }

    if (state == BREWING)
    {
        // while brewing, no input is accepted
        // check if timed-brew is done or brew switch is off
        if (brewSwitch.read() == brewSwitch.RELEASED || timedBrewFinished())
        {
            stopBrew();
        }
        return;
    }
    else if (state == FINISHED_BREWING)
    {
        // we wait until the brew switch is reset or 5 seconds before returning to WAITING mode;

        // blink while in FINISHED_BREWING_STATE and the switch is still enabled
        if (brewSwitch.read() == brewSwitch.PRESSED){
            int blink = (millis()/512)%2;
            digitalWrite(LED_PIN, blink);
        }

        if (millis() > (finishedBrewAt + 5000l))
        {
            if (brewSwitch.read() == brewSwitch.RELEASED)
            {
                state = WAITING;
            }
            Serial.printf("%i\n", brewSwitch.read());
        }
        if (brewSwitch.pressed()){ //allow instant re-brew
            startBrew();
        }
        return;
    }
    else
    {
        // state can be WAITING or SLEEPING
        // we accept button pushes and start of brew cycle
        if (brewSwitch.read() == brewSwitch.PRESSED)
        {
            startBrew();
            return;
        }
        if (state == WAITING)
        {
            if (millis() > lastChange + 10000)
            {
                state = SLEEPING;
                saveState();
            }
        }
        else
        {
            if (millis() < lastChange + 10000)
            {
                state = WAITING;
            }
        }
    }
}