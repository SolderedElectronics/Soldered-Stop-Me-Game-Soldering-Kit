#ifndef __GAME_H__
#define __GAME_H___

#include "stm32c0xx_hal.h"
#include "stdint.h"
#include "defines.h"

void gameSetLeds(uint8_t _led);
void gameUpdateLedBuffer();
void gameShowStartAnimation();
void gameShowSuccAnimation(void);
void gameShowFailAnimation(void);
void gameShowResult(uint8_t _r);
void gameTimerISR();

#endif
