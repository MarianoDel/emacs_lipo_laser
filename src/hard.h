/*
 * hard.h
 *
 *  Created on: 28/11/2013
 *      Author: Mariano
 */

#ifndef _HARD_H_
#define _HARD_H_

#include "stm32f0xx.h"


//-- Defines For Configuration -------------------
//---- Configuration for Hardware Versions -------
#define VER_1_0

//---- Configuration for Firmware ----------------
#define FIRST_POWER_BOARD
// #define SECOND_POWER_BOARD

//------ Configuration for Firmware-Channels -----


//-------- Configuration for Outputs-Channels -----


//---- Configuration for Firmware-Programs --------


//-------- Configuration for Outputs-Firmware ------


//-- End Of Defines For Configuration ---------------

//GPIOA pin0
//GPIOA pin1
//GPIOA pin2
//GPIOA pin3    4 ADC channels

//GPIOA pin4
#define TEST_P1 ((GPIOA->ODR & 0x0010) != 0)
#define TEST_P1_ON GPIOA->BSRR = 0x00000010
#define TEST_P1_OFF GPIOA->BSRR = 0x00100000

//GPIOA pin5
#define TEST_P2 ((GPIOA->ODR & 0x0020) != 0)
#define TEST_P2_ON GPIOA->BSRR = 0x00000020
#define TEST_P2_OFF GPIOA->BSRR = 0x00200000

//GPIOA pin6
//GPIOA pin7
//GPIOB pin0
//GPIOB pin1    TIM3 CH1 - CH4

//GPIOA pin8    NC

//GPIOA pin9
//GPIOA pin10    USART1

//GPIOA pin11    NC

//GPIOA pin12
#define LED ((GPIOA->ODR & 0x1000) != 0)
#define LED_ON GPIOA->BSRR = 0x00001000
#define LED_OFF GPIOA->BSRR = 0x10000000

//GPIOA pin13
//GPIOA pin14    NC

//GPIOA pin15    LASER CH4
#define LASER_CH4 ((GPIOA->ODR & 0x8000) != 0)
#define LASER_CH4_OFF GPIOA->BSRR = 0x00008000
#define LASER_CH4_ON GPIOA->BSRR = 0x80000000

//GPIOB pin3     LASER CH3
#define LASER_CH3 ((GPIOB->ODR & 0x0008) != 0)
#define LASER_CH3_OFF GPIOB->BSRR = 0x00000008
#define LASER_CH3_ON GPIOB->BSRR = 0x00080000

//GPIOB pin4     LASER CH2
#define LASER_CH2 ((GPIOB->ODR & 0x0010) != 0)
#define LASER_CH2_OFF GPIOB->BSRR = 0x00000010
#define LASER_CH2_ON GPIOB->BSRR = 0x00100000

//GPIOB pin5     LASER CH1
#define LASER_CH1 ((GPIOB->ODR & 0x0020) != 0)
#define LASER_CH1_OFF GPIOB->BSRR = 0x00000020
#define LASER_CH1_ON GPIOB->BSRR = 0x00200000

//GPIOB pin6
#define BUZZER ((GPIOB->ODR & 0x0040) != 0)
#define BUZZER_ON GPIOB->BSRR = 0x00000040
#define BUZZER_OFF GPIOB->BSRR = 0x00400000

//GPIOB pin7    NC


//ESTADOS DEL LED
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
} led_state_t;

//Estados Externos de LED BLINKING
#define LED_NO_BLINKING    0
#define LED_TREATMENT_STANDBY    1
#define LED_TREATMENT_GENERATING    2

#define LED_TREATMENT_ERROR    6


/* Module Functions ------------------------------------------------------------*/
void ChangeLed (unsigned char);
void UpdateLed (void);

#endif /* HARD_H_ */