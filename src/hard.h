/*
 * hard.h
 *
 *  Created on: 28/11/2013
 *      Author: Mariano
 */

#ifndef HARD_H_
#define HARD_H_

#include "stm32f0xx.h"


//-- Defines For Configuration -------------------
//---- Configuration for Hardware Versions -------
#define VER_1_0

//---- Configuration for Firmware ----------------


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


//ESTADOS DEL PROGRAMA PRINCIPAL
#define MAIN_INIT					0

#define MAIN_DMX_CHECK_CHANNEL				10
#define MAIN_DMX_CHECK_CHANNEL_B			11
#define MAIN_DMX_CHECK_CHANNEL_SELECTED		12
#define MAIN_DMX_CHECK_CHANNEL_S1			13
#define MAIN_DMX_CHECK_CHANNEL_S2			14
#define MAIN_DMX_SAVE_CONF					15
#define MAIN_DMX_NORMAL						16

#define MAIN_MAN_PX_CHECK			20
#define MAIN_MAN_PX_CHECK_B			21
#define MAIN_MAN_PX_CHECK_DEEP		22
#define MAIN_MAN_PX_CHECK_S1		23
#define MAIN_MAN_PX_CHECK_S2		24
#define MAIN_MAN_PX_SAVE_CONF		25
#define MAIN_MAN_PX_NORMAL			26

#define MAIN_TEMP_OVERLOAD			30
#define MAIN_TEMP_OVERLOAD_B		31

//---- Temperaturas en el LM335
//37	2,572
//40	2,600
//45	2,650
//50	2,681
//55	2,725
//60	2,765
#define TEMP_IN_30		3226
#define TEMP_IN_35		3279
#define TEMP_IN_50		3434
#define TEMP_IN_65		3591
#define TEMP_DISCONECT		4000

//ESTADOS DEL DISPLAY EN RGB_FOR_CAT
#define SHOW_CHANNELS	0
#define SHOW_NUMBERS	1

//ESTADOS DEL DISPLAY
#define DISPLAY_INIT		0
#define DISPLAY_SENDING		1
#define DISPLAY_SHOWING		2
#define DISPLAY_WAITING		3

#define DISPLAY_ZERO	10
#define DISPLAY_POINT	11
#define DISPLAY_LINE	12
#define DISPLAY_REMOTE	13
#define DISPLAY_PROG	14
#define DISPLAY_E		15
#define DISPLAY_C		16
#define DISPLAY_H		17
#define DISPLAY_1P		18
#define DISPLAY_S		19
#define DISPLAY_A		20
#define DISPLAY_T		21
#define DISPLAY_R		22
#define DISPLAY_G		23
#define DISPLAY_N		24
#define DISPLAY_NONE	0

#define DISPLAY_P	DISPLAY_PROG

#define DISPLAY_DS3		0x04
#define DISPLAY_DS2		0x02
#define DISPLAY_DS1		0x01

#define DISPLAY_TIMER_RELOAD	6		//166Hz / 3
#define SWITCHES_TIMER_RELOAD	10
#define BLINKING_UPDATE 	12			//son 18 ms de update * num elegido

#define SWITCHES_THRESHOLD_FULL	300		//3 segundos
#define SWITCHES_THRESHOLD_HALF	100		//1 segundo
#define SWITCHES_THRESHOLD_MIN	5		//50 ms

#define TTIMER_FOR_CAT_DISPLAY			2000	//tiempo entre que dice canal y el numero
#define TIMER_STANDBY_TIMEOUT_REDUCED	2000	//reduced 2 segs
#define TIMER_STANDBY_TIMEOUT			6000	//6 segundos
#define DMX_DISPLAY_SHOW_TIMEOUT		30000	//30 segundos

#define S_FULL		10
#define S_HALF		3
#define S_MIN		1
#define S_NO		0

#define FUNCTION_DMX	1
#define FUNCTION_MAN	2
#define FUNCTION_CAT	FUNCTION_MAN

#endif /* HARD_H_ */
