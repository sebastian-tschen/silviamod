
#include <Arduino.h>

#define COUNTER_MIN 1
#define COUNTER_MAX 99

int counter = 30;
int pinA;
int pinB;

void IRAM_ATTR read_encoder()
{
    noInterrupts(); // disable interrupts

    // Encoder interrupt routine for both pins. Updates counter
    // if they are valid and have rotated a full indent
    static uint8_t old_AB = 3;                                                               // Lookup table index
    static int8_t encval = 0;                                                                // Encoder value
    static const int8_t enc_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0}; // Lookup table

    old_AB <<= 2; // Remember previous state

    if (digitalRead(pinA))
        old_AB |= 0x02; // Add current state of pin A
    if (digitalRead(pinB))
        old_AB |= 0x01; // Add current state of pin B

    encval += enc_states[(old_AB & 0x0f)];

    // Update counter if encoder has rotated a full indent, that is at least 4 steps
    if (encval > 3)
    { // Four steps forward

        if (counter < COUNTER_MAX)
        {
            counter++; // Increase counter
        }
        encval = 0;
    }
    else if (encval < -3)
    { // Four steps backwards

        if (counter > COUNTER_MIN)
        {
            counter--; // Decrease counter
        }
        encval = 0;
    }
    interrupts(); // disable interrupts
}

int getPosition()
{
    return counter;
}

int setPosition(int position)
{
    if (position < COUNTER_MIN)
    {
        counter = COUNTER_MIN;
    }
    else if (position > COUNTER_MAX)
    {
        counter = COUNTER_MAX;
    }
    else
    {
        counter = position;
    }
    return counter;
}

void setupRotary(int encoderPinA, int encoderPinB)
{

    // Set encoder pins and attach interrupts

    pinA = encoderPinA;
    pinB = encoderPinB;

    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pinA), read_encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pinB), read_encoder, CHANGE);
}