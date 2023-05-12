/**
 **************************************************
 *
 * @file        game.h
 * @brief       Header file for game code itself.
 *
 *
 * @note        In order to successfully run this code, make sure to use STM32 cube programmer
 * 				And set Option Bytes -> User Configuration -> NRST_MODE 2
 *
 * @authors     Borna Biro for soldered.com
 ***************************************************/

#ifndef __GAME_H__
#define __GAME_H___

// Include STM32 specific libraries.
#include "stm32c0xx_hal.h"
#include "stdint.h"

// Include defines for the game.
#include "defines.h"

void gameSetLeds(uint8_t _led);
void gameUpdateLedBuffer();
void gameShowStartAnimation();
void gameShowSuccAnimation(void);
void gameShowFailAnimation(void);
void gameShowResult(uint8_t _r);
void gameTimerISR();

#endif
