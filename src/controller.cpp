#include "controller.hpp"
#include "rotary.hpp"
#include <Arduino.h>
#include <Button.h>
#include <EEPROM.h>

#define EEPROM_SIZE 50

int saubernMemOffset = 8;
int preinfusionMemOffset = 20;
int position = 30; // rotary encoder position, marking seconds of brew time
unsigned long lastChange;
int state = WAITING;
int mode = TIME_MODE;
unsigned long startedBrewAt = 0;
unsigned long finishedBrewAt = 0;
bool pressDetected = false;
unsigned long pressDetectedAt = 0;
unsigned int LONG_PRESS_DURATION = 2000;
unsigned long saubernStartedAt = 0;

int saubernCycleCount = 7;
int saubernPumpS = 1;
int saubernWaitS = 7;

bool preinfusionEnabled=false;

int preinfusionPumpS = 2;
int preinfusionWaitS = 3;





result savePreinfusionState()
{

    int address = preinfusionMemOffset;
    int savedPreinfusionPumpS;
    int savedPreinfusionWaitS;
    int8 savedPreinfusionEnabledInt;
    int8 preinfusionEnabledInt = 0;
    if (preinfusionEnabled){
        preinfusionEnabledInt = 1;
    }

    EEPROM.begin(EEPROM_SIZE);

    EEPROM.get(address, savedPreinfusionPumpS);
    address += sizeof(savedPreinfusionPumpS);
    EEPROM.get(address, savedPreinfusionWaitS);
    address += sizeof(savedPreinfusionWaitS);
    EEPROM.get(address, savedPreinfusionEnabledInt);

    if (savedPreinfusionPumpS == preinfusionPumpS && savedPreinfusionWaitS == preinfusionWaitS && savedPreinfusionEnabledInt == preinfusionEnabledInt)
    {
        Serial.printf("no save nessecary\n");
    }
    else
    {

        Serial.print("saving preinfusionPumpS: ");
        Serial.println(preinfusionPumpS);
        Serial.print("saving preinfusionWaitS: ");
        Serial.println(preinfusionWaitS);
        Serial.print("saving preinfusionEnabled: ");
        Serial.println(preinfusionEnabled);
        int address = preinfusionMemOffset;
        EEPROM.put(address, preinfusionPumpS);
        address += sizeof(preinfusionPumpS);
        EEPROM.put(address, preinfusionWaitS);
        address += sizeof(preinfusionWaitS);
        EEPROM.put(address, preinfusionEnabledInt);
        address += sizeof(preinfusionEnabledInt);
        EEPROM.commit();
        Serial.printf("written up to byte %i into flash\n", address);
    }
    EEPROM.end();
    MENU_ON = false;
    lastChange = millis();
    return proceed;
}

result saveCleanState()
{

    int address = saubernMemOffset;
    int savedCycles;
    int savedPumpS;
    int savedWaitS;
    EEPROM.begin(EEPROM_SIZE);

    EEPROM.get(address, savedCycles);
    address += sizeof(savedCycles);
    EEPROM.get(address, savedPumpS);
    address += sizeof(savedPumpS);
    EEPROM.get(address, savedWaitS);

    if (savedWaitS == saubernWaitS && savedPumpS == saubernPumpS && savedCycles == saubernCycleCount)
    {
        Serial.printf("no save nessecary\n");
    }
    else
    {
        int address = saubernMemOffset;
        EEPROM.put(address, saubernCycleCount);
        address += sizeof(saubernCycleCount);
        EEPROM.put(address, saubernPumpS);
        address += sizeof(saubernPumpS);
        EEPROM.put(address, saubernWaitS);
        address += sizeof(saubernWaitS);
        EEPROM.commit();
        Serial.printf("written %i bytes to flash\n", address);
    }
    EEPROM.end();
    MENU_ON = false;
    lastChange = millis();
    return proceed;
}

result saveState()
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
    MENU_ON = false;
    lastChange = millis();
    return quit;
}

result startSaubern()
{
    Serial.println("starting saubern");
    state = SAUBERN;
    saubernStartedAt = millis();

    MENU_ON = false;
    lastChange = millis();
    return quit;
}
void stopSaubern()
{
    Serial.println("stopping saubern");
    state = WAITING;

    MENU_ON = true;
    lastChange = millis();
    menuNav.idleOff();
}

void loadState()
{
    int address = 0;
    EEPROM.begin(EEPROM_SIZE);

    // load state and timer config
    EEPROM.get(address, mode);
    address += sizeof(mode);
    EEPROM.get(address, position);
    address += sizeof(position);

    // load cleaning cycle data
    EEPROM.get(address, saubernCycleCount);
    address += sizeof(saubernCycleCount);
    EEPROM.get(address, saubernPumpS);
    address += sizeof(saubernPumpS);
    EEPROM.get(address, saubernWaitS);
    address += sizeof(saubernWaitS);

    int8 preinfusionEnabledInt=0;
    // load preinfusion config data
    EEPROM.get(address, preinfusionPumpS);
    address += sizeof(preinfusionPumpS);
    EEPROM.get(address, preinfusionWaitS);
    address += sizeof(preinfusionWaitS);
    EEPROM.get(address, preinfusionEnabledInt);
    address += sizeof(preinfusionEnabledInt);

    EEPROM.end();
    Serial.printf("read %i bytes from flash\n", address);

    if (mode != TIME_MODE && mode != OPEN_MODE)
    {
        mode = OPEN_MODE;
    }
    // set useful default values for init
    Serial.print("loaded saubernCycleCount: ");
    Serial.println(saubernCycleCount);
    Serial.print("loaded saubernPumpS: ");
    Serial.println(saubernPumpS);
    Serial.print("loaded saubernWaitS: ");
    Serial.println(saubernWaitS);

    Serial.print("loaded preinfusionPumpS: ");
    Serial.println(preinfusionPumpS);
    Serial.print("loaded preinfusionWaitS: ");
    Serial.println(preinfusionWaitS);
    Serial.print("loaded preinfusionEnabledInt: ");
    Serial.println(preinfusionEnabledInt);
    if ((saubernCycleCount > 20) | (saubernCycleCount < 1))
    {
        saubernCycleCount = 7;
    }
    if ((saubernPumpS > 20) | (saubernPumpS < 1))
    {
        saubernPumpS = 5;
    }
    if ((saubernWaitS > 20) | (saubernWaitS < 1))
    {
        saubernWaitS = 5;
    }
    if ((preinfusionPumpS>10) | (preinfusionPumpS<1)){
        preinfusionPumpS = 2;
    }
    if ((preinfusionWaitS>10) | (preinfusionWaitS<1)){
        preinfusionWaitS = 3;
    }
    if (preinfusionEnabledInt!= 1){
        preinfusionEnabled = false;
        Serial.println("disabling preinfusion");
    }else{
        preinfusionEnabled = true;
        Serial.println("enabling preinfusion");
    }
    Serial.println(preinfusionEnabled);
    // if (savePre){
    //     savePreinfusionState();
    // }

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

bool inPreinfusionWindow()
{

    unsigned long window_start = startedBrewAt + preinfusionPumpMS;
    unsigned long window_end = window_start + preinfusionWaitMS;
    unsigned long now = millis();

    return ((now > window_start) && (now < window_end));
}

void switchLED(int on)
{
    if (on == 0)
    {
        pinMode(LED_PIN, INPUT);
    }
    else
    {
        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_PIN, 0);
    }
}

void startBrew()
{
    Serial.println("starting Brew");
    digitalWrite(VALVE_PIN, 0);
    digitalWrite(PUMP_PIN, 0);
    startedBrewAt = millis();
    switchLED(1);
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
    switchLED(0);
    Serial.println(state);
}

void startPump()
{
    Serial.println("starting Pump");
    digitalWrite(PUMP_PIN, 0);
}

void stopPump()
{
    Serial.println("stopping Pump");
    digitalWrite(PUMP_PIN, 1);
}

void goToSleep()
{

    state = SLEEPING;
}

void initControl()
{
    loadState();
    if (brewSwitch.read() == brewSwitch.PRESSED)
    {
        mode = TIME_MODE;
        state = FINISHED_BREWING;
        brewSwitch.has_changed(); // reset has_changed flag
    }
}

void control()
{
    if (position != getPosition())
    {
        if (MENU_ON)
        {
            int change = getPosition() - position;
            if (change > 0)
            {
                Serial.print("+");
                navigationInput.write('+');
                // navigationInput.write('X');
                Serial.println("/");
            }
            else
            {
                Serial.print("-");
                navigationInput.write('-');
                // navigationInput.write('X');
                Serial.println("/");
            }
            setPosition(position); // reset internal count;
        }
        else
        {
            if (mode == TIME_MODE && state != FINISHED_BREWING)
            {
                position = getPosition();
            }
            else
            {
                // don't allow change of time during open mode
                setPosition(position);
            }
        }
        lastChange = millis();
    }

    if (rotaryButton.pressed())
    {
        if (MENU_ON)
        {
            Serial.print("*");
            navigationInput.write('*');
            Serial.println("/");
        }
        else
        {
            switchMode();
        }
        pressDetected = true;
        pressDetectedAt = millis();
    }

    if (pressDetected)
    {
        if (rotaryButton.read() == rotaryButton.RELEASED)
        {
            pressDetected = false;
        }
        else
        {
            if ((millis() - pressDetectedAt) > LONG_PRESS_DURATION)
            {
                if (!MENU_ON)
                {
                    MENU_ON = true;
                    menuNav.idleOff();
                    Serial.println("entering menu due to long press");
                    pressDetected = false;
                    return;
                }
            }
        }
    }

    if (state == SAUBERN)
    {

        // find out where we are in the saubern cycle
        int timeInSaubern = millis() - saubernStartedAt;
        int cycleDuration = (saubernPumpMS + saubernWaitMS);
        int saubernCycle = timeInSaubern / cycleDuration;
        int timeIncycle = (timeInSaubern - (saubernCycle * cycleDuration));

        // Serial.print("timeInSaubern: ");
        // Serial.println(timeInSaubern);
        // Serial.print("cycleDuration: ");
        // Serial.println(cycleDuration);
        // Serial.print("saubernCycle: ");
        // Serial.println(saubernCycle);
        // Serial.print("timeIncycle: ");
        // Serial.println(timeIncycle);
        // Serial.println(timeIncycle - saubernPumpMS);
        if (timeIncycle - saubernPumpMS > 0)
        {

            Serial.println("no saubern");
            // we should be waiting
            digitalWrite(VALVE_PIN, 1);
            digitalWrite(PUMP_PIN, 1);
            if (saubernCycle >= saubernCycleCount)
            {
                stopSaubern();
                return;
            }
        }
        else
        {
            // we should be pumping
            Serial.println("do saubern");
            digitalWrite(VALVE_PIN, 0);
            digitalWrite(PUMP_PIN, 0);
        }
        return;
    }

    if (state == BREWING)
    {
        // while brewing, no input is accepted
        // check if we are doing a pre-infusion and need to switch the pump separately
        if (preinfusionEnabled == true)
        {
            if (inPreinfusionWindow())
            {
                stopPump();
            }
            else
            {
                startPump();
            }
        }

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
        if (brewSwitch.read() == brewSwitch.PRESSED)
        {
            int blink = (millis() / 512) % 2;
            switchLED(blink);
        }

        if (millis() > (finishedBrewAt + 5000l))
        {
            if (brewSwitch.read() == brewSwitch.RELEASED)
            {
                state = WAITING;
                switchLED(0);
            }
            Serial.printf("%i\n", brewSwitch.read());
        }
        if (brewSwitch.pressed())
        { // allow instant re-brew
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
        if (state == SLEEPING)
        {
            if (millis() < lastChange + SLEEP_AFTER_MS)
            {
                state = WAITING; // something happend... wake up
            }
        }
        else
        {
            if (millis() > lastChange + SLEEP_AFTER_MS)
            {
                goToSleep();
            }
        }
    }
}