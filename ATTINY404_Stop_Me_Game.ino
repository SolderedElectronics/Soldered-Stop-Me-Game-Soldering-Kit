/**
 **************************************************
 *
 * @file        ATTINY404_Stop_Me_Game.ino
 * @brief       Arduino code for Stop Me Game
 *              Soldering Kit from Soldered.
 *
 * @note        In order to use this code, install megaTinyCore from https://github.com/SpenceKonde/megaTinyCore
 *              Core version: 2.4.2
 *              Chip: ATtiny404
 *              Clock: 1MHz Internal
 *              millis()/micros Timer: Disabled (saves flash)
 *
 * @authors     Borna Biro for soldered.com
 ***************************************************/

// PORTA pins
#define NET_D0 4
#define NET_D1 5
#define NET_D2 6
#define NET_D3 7
#define NET_D4 2

#define REACTION_TIME_STEP1 10
#define REACTION_TIME_STEP2 5
#define REACTION_TIME_STEP3 1

#define SCORE_BLINK_ON  200
#define SCORE_BLINK_OFF 200

volatile uint8_t ledBufferCharile = 0;
volatile uint8_t charlieplexCounter = 0;
volatile uint8_t leds[6][2] = {{1 << NET_D2, 1 << NET_D4}, {1 << NET_D0, 1 << NET_D4}, {1 << NET_D2, 1 << NET_D0},
                               {1 << NET_D4, 1 << NET_D2}, {1 << NET_D4, 1 << NET_D0}, {1 << NET_D0, 1 << NET_D2}};
volatile uint8_t outputsMask = (1 << NET_D0) | (1 << NET_D2) | (1 << NET_D4);

uint8_t down = 0;
uint8_t ledBuffer;
uint8_t score = 0;
uint16_t delayTime = 250;
uint16_t countdown = 0;
int i = 0;

void setup()
{
    PORTA.DIRSET = 1 << NET_D3;
    pinMode(PIN_PA5, INPUT_PULLUP);
    initTimer();
    showStartAnimation();
}

void loop()
{
    uint8_t buttonIsPressed;
    ledBuffer = (1 << i);
    updateLedBuffer();
    if (i >= 6)
    {
        down = 1;
    }

    if (i == 0)
    {
        down = 0;
    }
    countdown = delayTime;
    do
    {
        buttonIsPressed = digitalRead(PIN_PA5) == LOW ? 1 : 0;
        delay(1);
        countdown--;
    } while ((countdown > 0) && (buttonIsPressed == 0));

    if (buttonIsPressed)
    {
        while (!digitalRead(PIN_PA5))
            ;
        if (i == 3)
        {
            // i = 0;
            showSuccAnimation();
            score++;
            // delay_ms(250);
            if (delayTime > 100)
            {
                delayTime -= REACTION_TIME_STEP1;
            }
            else if ((delayTime <= 100) && (delayTime > 30))
            {
                delayTime -= REACTION_TIME_STEP2;
            }
            else if ((delayTime <= 30) && (delayTime > 5))
            {
                delayTime -= REACTION_TIME_STEP3;
            }
        }
        else
        {
            i = 0;
            delay(500);
            showFailAnimation();
            delay(500);
            showResult(score);
            delay(500);
            score = 0;
            delayTime = 250;
            showStartAnimation();
        }
    }
    else
    {
        down == 0 ? i++ : i--;
    }
}

void initTimer()
{
    TCB0.CCMP = 999; // f = 1MHz, prescaler = 2, fINT = (1E6)/(2 * (999 + 1) = 500 Hz
    TCB0.CTRLA = TCB_CLKSEL_CLKDIV2_gc | TCB_ENABLE_bm;
    TCB0.CTRLB = TCB_CNTMODE_INT_gc;
    TCB0.INTCTRL = TCB_CAPT_bm;
}

void updateLedBuffer()
{
    if (ledBuffer & (1 << 3))
    {
        PORTA.OUTSET = 1 << NET_D3;
    }
    else
    {
        PORTA.OUTCLR = 1 << NET_D3;
    }

    ledBufferCharile = (ledBuffer & 0b00000111) | ((ledBuffer & 0b01110000) >> 1);
}

void showStartAnimation()
{
    int i;
    int j;
    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < 7; i++)
        {
            ledBuffer |= (1 << i);
            updateLedBuffer();
            delay(100);
        }
        for (i = 0; i < 7; i++)
        {
            ledBuffer &= ~(1 << i);
            updateLedBuffer();
            delay(100);
        }
    }
    delay(1000);
}

void showSuccAnimation(void)
{
    int i;
    ledBuffer = 0;
    updateLedBuffer();
    delay(250);
    for (i = 0; i < 1; i++)
    {
        ledBuffer = 1 << 3;
        updateLedBuffer();
        delay(250);
        ledBuffer = 0;
        updateLedBuffer();
        delay(250);
    }
}

void showFailAnimation(void)
{
    int i;
    for (i = 0; i < 2; i++)
    {
        ledBuffer = 0b01111111;
        updateLedBuffer();
        delay(250);
        ledBuffer = 0;
        updateLedBuffer();
        delay(250);
    }
}

void showResult(uint8_t _r)
{
    uint8_t _blinks;
    uint8_t i;
    _blinks = _r / 100;
    for (i = 0; i < _blinks; i++)
    {
        ledBuffer = 1 << 0;
        updateLedBuffer();
        delay(SCORE_BLINK_ON);
        ledBuffer = 0;
        updateLedBuffer();
        delay(SCORE_BLINK_OFF);
    }

    _blinks = _r / 10 % 10;
    for (i = 0; i < _blinks; i++)
    {
        ledBuffer = 1 << 1;
        updateLedBuffer();
        delay(SCORE_BLINK_ON);
        ledBuffer = 0;
        updateLedBuffer();
        delay(SCORE_BLINK_OFF);
    }

    _blinks = _r % 10;
    for (i = 0; i < _blinks; i++)
    {
        ledBuffer = 1 << 2;
        updateLedBuffer();
        delay(SCORE_BLINK_ON);
        ledBuffer = 0;
        updateLedBuffer();
        delay(SCORE_BLINK_OFF);
    }
}

ISR(TCB0_INT_vect)
{
    // Because we need to do this as quickly as possible, we need to use registers instead of Arduino digitalWrite() and
    // pinMode() functions.

    // First, set all outputs connected to LEDs as inputs
    PORTA.DIRCLR = outputsMask;

    // Then check if you need to light up an LED
    if (ledBufferCharile & (1 << charlieplexCounter))
    {
        PORTA.OUTSET = leds[charlieplexCounter][0];
        PORTA.OUTCLR = leds[charlieplexCounter][1];
        PORTA.DIRSET = leds[charlieplexCounter][0] | leds[charlieplexCounter][1];
    }
    charlieplexCounter++;
    if (charlieplexCounter > 5)
        charlieplexCounter = 0;
    TCB0.INTFLAGS = TCB_CAPT_bm;
}
