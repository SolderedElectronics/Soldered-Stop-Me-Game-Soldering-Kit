#ifndef __DEFINES_H__
#define __DEFINES_H__

// GPIO pins defines
#define NET_D0_PORT GPIOB
#define NET_D0_PIN 7
#define NET_D1_PORT GPIOA
#define NET_D1_PIN 13
#define NET_D2_PORT GPIOA
#define NET_D2_PIN 12
#define NET_D3_PORT GPIOA
#define NET_D3_PIN 11
#define NET_D4_PORT GPIOA
#define NET_D4_PIN 0

// Defined steps for lowering the delay between light movement (in milliseconds)
#define REACTION_TIME_STEP1 6
#define REACTION_TIME_STEP2 3
#define REACTION_TIME_STEP3 1

// Start delay time between steps (in milliseconds)
#define DEFAULT_DELAY_TIME 350

// Delay for showing the score (blink delay)
#define SCORE_BLINK_ON  200
#define SCORE_BLINK_OFF 200

#endif
