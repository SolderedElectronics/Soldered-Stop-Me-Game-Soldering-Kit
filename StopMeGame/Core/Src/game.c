#include "game.h"

// Buffer for LEDs that needs to be charlieplexed (each bit represents one LED)
volatile uint8_t ledBufferCharile = 0;

// Counter for charlieplexing
volatile uint8_t charlieplexCounter = 0;

// Struct needed for defining LEDs with MCU GPIO port and pin
struct ledPins
{
	GPIO_TypeDef *port;
	uint32_t pin;
};

// Struct used to define GPIO pairs needed to go HIGH and LOW to light up one LED in charlieplexing
struct ledPairs
{
	uint8_t gpioHigh;
	uint8_t gpioLow;
};

// GPIO Pins for LEDs (first is GPIO Port, senond is pin)
struct ledPins myLedPins[3] = {{NET_D0_PORT, NET_D0_PIN}, {NET_D2_PORT, NET_D2_PIN}, {NET_D4_PORT, NET_D4_PIN}};

// LED combinations for each LED. Each element in the array represents one LED in the charlieplex configuration.
// For example NET_D2 (second element in the myLedPins struct array) must be high, while NET_D4 (last element in the myLedPins struct array) must be low in order to light up L1 LED.
struct ledPairs myLedPairs[6] = {{1, 2}, {0, 2}, {1, 0}, {2, 1}, {2, 0}, {0, 1}};

// Variable for storing current state of LEDs.
uint8_t ledBuffer;

// Functions sets LED states (each bit represents one LED)
void gameSetLeds(uint8_t _led)
{
	ledBuffer = _led;
}

// Send new "screen" to the LEDs
void gameUpdateLedBuffer()
{
	// Check if the middle LED needs to change the state (this LED is different for the other LEDs, it's not charlieplexed)
    if (ledBuffer & (1 << 3))
    {
    	NET_D3_PORT->BSRR = (1 << NET_D3_PIN);
    }
    else
    {
    	NET_D3_PORT->BRR = (1 << NET_D3_PIN);
    }

    // Remove non-charlieplexed LED from the LED buffer for charlieplexing.
    ledBufferCharile = (ledBuffer & 0b00000111) | ((ledBuffer & 0b01110000) >> 1);
}

// Function just shows start animation (first all LEDs light up from left to right, then all LEDs turn of in the same way as they light up)
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
            HAL_Delay(70);
        }
        for (i = 0; i < 7; i++)
        {
            ledBuffer &= ~(1 << i);
            gameUpdateLedBuffer();
            HAL_Delay(70);
        }
    }
    HAL_Delay(700);
}

// Function shows success animation (blinks with a LED in the middle)
void gameShowSuccAnimation(void)
{
    int i;
    ledBuffer = 0;
    gameUpdateLedBuffer();
    HAL_Delay(175);
    for (i = 0; i < 1; i++)
    {
        ledBuffer = 1 << 3;
        gameUpdateLedBuffer();
        HAL_Delay(175);
        ledBuffer = 0;
        gameUpdateLedBuffer();
        HAL_Delay(175);
    }
}

// Function shows fail animation (when player misses LED). It just blinks few times with the missed LED.
void gameShowFailAnimation(void)
{
    int i;
    for (i = 0; i < 2; i++)
    {
        ledBuffer = 0b01111111;
        gameUpdateLedBuffer();
        HAL_Delay(175);
        ledBuffer = 0;
        gameUpdateLedBuffer();
        HAL_Delay(175);
    }
}

// Function shows result of the game. L1 shows hundreds of the result, L2 LED show tenths of the result.
// Each LED blink means the result must me contued up.
// For example, L1 blinks just once, L2 blinks 3 times, and L3 five times, total score is 135.
void gameShowResult(uint8_t _r)
{
    uint8_t _blinks;
    uint8_t i;

    // First show hundreds of the score
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

    // Show tenths of the result
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

    // Show the last digit of the result
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

// Function is for charlieplexing LEDs (this function is called periodically with the HW timer)
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
    	// Set one GPIO to the high and other to the low state
    	myLedPins[myLedPairs[charlieplexCounter].gpioHigh].port->BSRR = (1 << (myLedPins[myLedPairs[charlieplexCounter].gpioHigh].pin));
    	myLedPins[myLedPairs[charlieplexCounter].gpioLow].port->BRR = (1 << (myLedPins[myLedPairs[charlieplexCounter].gpioLow].pin));

    	// Set GPIOs as outputs.
    	myLedPins[myLedPairs[charlieplexCounter].gpioHigh].port->MODER |= (1 << (myLedPins[myLedPairs[charlieplexCounter].gpioHigh].pin) * 2);
    	myLedPins[myLedPairs[charlieplexCounter].gpioLow].port->MODER |= (1 << (myLedPins[myLedPairs[charlieplexCounter].gpioLow].pin) * 2);
    }

    // Go to the next LED
    charlieplexCounter++;

    // There are only 5 charileplexed LED.
    if (charlieplexCounter > 5)
        charlieplexCounter = 0;
}
