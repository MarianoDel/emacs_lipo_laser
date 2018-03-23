/**
  ******************************************************************************
  * @file    Template_2/main.c
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Use this template for new projects with stm32f0xx family.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hard.h"
#include "stm32f0xx.h"
#include "gpio.h"
#include "uart.h"

#include "core_cm0.h"
#include "adc.h"
#include "tim.h"

//#include <stdio.h>
//#include <string.h>




//--- VARIABLES EXTERNAS ---//
volatile unsigned char timer_1seg = 0;

// ------- Externals del USART -------
volatile unsigned char usart1_have_data;


//--- VARIABLES GLOBALES ---//


// ------- de los timers -------
volatile unsigned short timer_standby;
volatile unsigned short wait_ms_var = 0;

//--- FUNCIONES DEL MODULO ---//
void TimingDelay_Decrement(void);


// ------- del DMX -------
extern void EXTI4_15_IRQHandler(void);



//--- FILTROS DE SENSORES ---//
#define LARGO_FILTRO 16
#define DIVISOR      4   //2 elevado al divisor = largo filtro
//#define LARGO_FILTRO 32
//#define DIVISOR      5   //2 elevado al divisor = largo filtro
unsigned short vtemp [LARGO_FILTRO + 1];
unsigned short vpote [LARGO_FILTRO + 1];

//--- FIN DEFINICIONES DE FILTRO ---//


//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    unsigned short i = 0;
    unsigned char main_state = 0;

    char s_to_senda [100];
    char s_to_sendb [100];
    
    unsigned char bytes_readed = 0;
    unsigned char last_program, last_program_deep;
    unsigned short last_channel;
    unsigned short current_temp = 0;

    //GPIO Configuration.
    GPIO_Config();

    //TIM Configuration.
    // TIM_3_Init();
    // TIM_14_Init();

    //ACTIVAR SYSTICK TIMER
    if (SysTick_Config(48000))
    {
        while (1)	/* Capture error */
        {
            if (LED)
                LED_OFF;
            else
                LED_ON;

            for (i = 0; i < 255; i++)
            {
                asm (	"nop \n\t"
                        "nop \n\t"
                        "nop \n\t" );
            }
        }
    }

    //prueba PWM con TIM3
    TIM_3_Init();

    Update_TIM3_CH1(0);
    Update_TIM3_CH2(0);
    Update_TIM3_CH3(0);

    while (1)
    {
        for (i = 0; i < 1023; i++)
        {
            Update_TIM3_CH4(i);
            Wait_ms(10);
        }
    }
    // fin prueba int timer 14 y SOFT_PWM


    // Update_TIM3_CH1(511);
    // Update_TIM3_CH2(511);
    // Update_TIM3_CH3(511);
    // Update_TIM3_CH4(511);
    // while (1);
    
    // //prueba int timer 14 y SOFT_PWM
    // TIM_14_Init();

    // // UpdateLaserCh1(127);
    // // UpdateLaserCh2(127);
    // // UpdateLaserCh3(127);
    // // UpdateLaserCh4(127);

    // // while (1);


    // while (1)
    // {
    //     for (i = 0; i < 255; i++)
    //     {
    //         UpdateLaserCh1(i);
    //         UpdateLaserCh2(i);
    //         UpdateLaserCh3(i);
    //         UpdateLaserCh4(i);

    //         Wait_ms(100);
    //     }
    // }
    // fin prueba int timer 14 y SOFT_PWM

    //prueba loop (Tx - Rx) en usart1
    USART1Config();
    while (1)
    {
        if (usart1_have_data)
        {
            usart1_have_data = 0;
            bytes_readed = ReadUsart1Buffer(s_to_senda, sizeof(s_to_senda));

            if ((bytes_readed + 1) < sizeof(s_to_senda))
            {
                *(s_to_senda + bytes_readed - 1) = '\n';
                *(s_to_senda + bytes_readed) = '\0';
                timer_standby = 1000;
            }

            if (LED)
                LED_OFF;
            else
                LED_ON;
        }

        if ((!timer_standby) && (bytes_readed > 0))
        {
            bytes_readed = 0;
            Usart1Send(s_to_senda);
        }
    }
    //fin prueba loop (Tx - Rx) en usart1

    //Prueba USART TX
    USART1Config();

    while (1)
    {
        LED_ON;
        Usart1Send("prueba\n");
        Wait_ms(100);
        LED_OFF;
        Wait_ms(1900);
    }
    //Fin Prueba USART TX

    //PRUEBA LED Y BUZZER
    while (1)
    {
        if (BUZZER)
        {
            LED_OFF;
            BUZZER_OFF;
        }
        else
        {
            LED_ON;
            BUZZER_ON;
        }
        Wait_ms(200);

        // LED_ON;
        // BUZZER_ON;
        // Wait_ms(150);
        // LED_OFF;
        // BUZZER_OFF;
        // Wait_ms(2000);
    }
    //FIN PRUEBA LED Y BUZZER


    return 0;
}


//--- End of Main ---//

// void EXTI4_15_IRQHandler(void)		//nueva detecta el primer 0 en usart Consola PHILIPS
// {
// 	unsigned short aux;

// 	if(EXTI->PR & 0x0100)	//Line8
// 	{
// 		//si no esta con el USART detecta el flanco	PONER TIMEOUT ACA?????
// 		if ((dmx_receive_flag == 0) || (dmx_timeout_timer == 0))
// 		//if (dmx_receive_flag == 0)
// 		{
// 			switch (signal_state)
// 			{
// 				case IDLE:
// 					if (!(DMX_INPUT))
// 					{
// 						//Activo timer en Falling.
// 						TIM14->CNT = 0;
// 						TIM14->CR1 |= 0x0001;
// 						signal_state++;
// 					}
// 					break;

// 				case LOOK_FOR_BREAK:
// 					if (DMX_INPUT)
// 					{
// 						//Desactivo timer en Rising.
// 						aux = TIM14->CNT;

// 						//reviso BREAK
// 						//if (((tim_counter_65ms) || (aux > 88)) && (tim_counter_65ms <= 20))
// 						//if ((aux > 87) && (aux < 210))	//Consola STARLET 6
// 						//if ((aux > 87) && (aux < 2000))		//Consola marca CODE tiene break 1.88ms
// 						if ((aux > 87) && (aux < 4600))		//Consola marca CODE modelo A24 tiene break 4.48ms fecha 11-04-17
// 						{
// 							//LED_ON;		//TODO: apaga para pruebas
// 							signal_state++;
// 							DMX_channel_received = 0;
// 							dmx_timeout_timer = DMX_TIMEOUT;		//activo el timeout para esperar un MARK valido
// 						}
// 						else	//falso disparo
// 							signal_state = IDLE;
// 					}
// 					else	//falso disparo
// 						signal_state = IDLE;

// 					TIM14->CR1 &= 0xFFFE;
// 					break;

// 				case LOOK_FOR_MARK:
// 					if ((!(DMX_INPUT)) && (dmx_timeout_timer))	//termino Mark after break
// 					{
// 						//ya tenia el serie habilitado
// 						dmx_receive_flag = 1;
// 						LED_ON;
// 					}
// 					else	//falso disparo
// 					{
// 						//termine por timeout
// 						dmx_receive_flag = 0;
// 					}
// 					signal_state = IDLE;
// 					//LED_OFF;						//TODO: apaga para pruebas
// 					break;

// 				default:
// 					signal_state = IDLE;
// 					break;
// 			}
// 		}
// 		EXTI->PR |= 0x0100;
// 	}
// }

void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

}
