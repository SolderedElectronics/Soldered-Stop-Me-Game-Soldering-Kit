#include "game.h"

volatile uint8_t ledBufferCharile = 0;
volatile uint8_t charlieplexCounter = 0;

struct ledPins
{
	GPIO_TypeDef *port;
	uint32_t pin;
};

struct ledPairs
{
	uint8_t led1;
	uint8_t led2;
};

struct ledPins myLedPins[3] = {{NET_D0_PORT, NET_D0_PIN}, {NET_D2_PORT, NET_D2_PIN}, {NET_D4_PORT, NET_D4_PIN}};
struct ledPairs myLedPairs[6] = {{1, 2}, {0, 2}, {1, 0}, {2, 1}, {2, 0}, {0, 1}};

uint8_t ledBuffer;

void gameSetLeds(uint8_t _led)
{
	ledBuffer = _led;
}

void gameUpdateLedBuffer()
{
    if (ledBuffer & (1 << 3))
    {
    	NET_D3_PORT->BSRR = (1 << NET_D3_PIN);
    }
    else
    {
    	NET_D3_PORT->BRR = (1 << NET_D3_PIN);
    }

    ledBufferCharile = (ledBuffer & 0b00000111) | ((ledBuffer & 0b01110000) >> 1);
}

void gameShowStartAnimation()
{
    int i;
    int j;
    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < 7; i++)
        {
            ledBuffer |= (1 << i);
            gameUpdateLedBuffer();
            HAL_Delay(100);
        }
        for (i = 0; i < 7; i++)
        {
            ledBuffer &= ~(1 << i);
            gameUpdateLedBuffer();
            HAL_Delay(100);
        }
    }
    HAL_Delay(1000);
}

void gameShowSuccAnimation(void)
{
    int i;
    ledBuffer = 0;
    gameUpdateLedBuffer();
    HAL_Delay(250);
    for (i = 0; i < 1; i++)
    {
        ledBuffer = 1 << 3;
        gameUpdateLedBuffer();
        HAL_Delay(250);
        ledBuffer = 0;
        gameUpdateLedBuffer();
        HAL_Delay(250);
    }
}

void gameShowFailAnimation(void)
{
    int i;
    for (i = 0; i < 2; i++)
    {
        ledBuffer = 0b01111111;
        gameUpdateLedBuffer();
        HAL_Delay(250);
        ledBuffer = 0;
        gameUpdateLedBuffer();
        HAL_Delay(250);
    }
}

void gameShowResult(uint8_t _r)
{
    uint8_t _blinks;
    uint8_t i;
    _blinks = _r / 100;
    for (i = 0; i < _blinks; i++)
    {
        ledBuffer = 1 << 0;
        gameUpdateLedBuffer();
        HAL_Delay(SCORE_BLINK_ON);
        ledBuffer = 0;
        gameUpdateLedBuffer();
        HAL_Delay(SCORE_BLINK_OFF);
    }

    _blinks = _r / 10 % 10;
    for (i = 0; i < _blinks; i++)
    {
        ledBuffer = 1 << 1;
        gameUpdateLedBuffer();
        HAL_Delay(SCORE_BLINK_ON);
        ledBuffer = 0;
        gameUpdateLedBuffer();
        HAL_Delay(SCORE_BLINK_OFF);
    }

    _blinks = _r % 10;
    for (i = 0; i < _blinks; i++)
    {
        ledBuffer = 1 << 2;
        gameUpdateLedBuffer();
        HAL_Delay(SCORE_BLINK_ON);
        ledBuffer = 0;
        gameUpdateLedBuffer();
        HAL_Delay(SCORE_BLINK_OFF);
    }
}

void gameTimerISR()
{
    // Because we need to do this as quickly as possible, so we need to use registers

    // First, set all outputs connected to LEDs as inputs. Do not use HAL here, it's too slow.
	myLedPins[0].port->MODER &= ~(0b00000011 << (2 * myLedPins[0].pin));
	myLedPins[1].port->MODER &= ~(0b00000011 << (2 * myLedPins[1].pin));
	myLedPins[2].port->MODER &= ~(0b00000011 << (2 * myLedPins[2].pin));

    // Then check if you need to light up an LED
    if (ledBufferCharile & (1 << charlieplexCounter))
    {
    	myLedPins[myLedPairs[charlieplexCounter].led1].port->BSRR = (1 << (myLedPins[myLedPairs[charlieplexCounter].led1].pin));
    	myLedPins[myLedPairs[charlieplexCounter].led2].port->BRR = (1 << (myLedPins[myLedPairs[charlieplexCounter].led2].pin));

    	myLedPins[myLedPairs[charlieplexCounter].led1].port->MODER |= (1 << (myLedPins[myLedPairs[charlieplexCounter].led1].pin) * 2);
    	myLedPins[myLedPairs[charlieplexCounter].led2].port->MODER |= (1 << (myLedPins[myLedPairs[charlieplexCounter].led2].pin) * 2);
    }
    charlieplexCounter++;
    if (charlieplexCounter > 5)
        charlieplexCounter = 0;
}
