//---------------------------------------------
// #### PROYECTO LIPO LASER - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### SIGNALS.C #############################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "signals.h"
#include "hard.h"
#include "stm32f0xx.h"
#include "tim.h"
#include "adc.h"
#include "dsp.h"
#include "uart.h"
#include "gpio.h"
#include <stdio.h>


//--- VARIABLES EXTERNAS ---//
//del ADC
extern volatile unsigned char seq_ready;
extern volatile unsigned short adc_ch[];

//del Main
extern volatile unsigned short timer_signals;
extern volatile unsigned short timer_signals_gen;

//--- VARIABLES GLOBALES ---//
treatment_t treatment_state = TREATMENT_INIT_FIRST_TIME;
signals_struct_t signal_to_gen;
cwave_state_t cwave_state = INIT_CWAVE;
pulsed_state_t pulsed_state = INIT_PULSED;
unsigned char global_error = 0;

unsigned short * p_signal;
unsigned short * p_signal_running;

short d = 0;

unsigned char protected = 0;

//-- para determinacion de soft overcurrent ------------
unsigned short soft_overcurrent_max_current_in_cycles [SIZEOF_OVERCURRENT_BUFF];
unsigned short soft_overcurrent_treshold = 0;
unsigned short soft_overcurrent_index = 0;


//Signals Templates
#define I_MAX 465
const unsigned short s_senoidal_1_5A [SIZEOF_SIGNALS] ={0,19,38,58,77,96,115,134,152,171,
														206,224,240,257,273,288,303,318,332,
														358,370,381,392,402,412,420,428,435,
														447,452,456,460,462,464,464,464,464,
														460,456,452,447,442,435,428,420,412,
														392,381,370,358,345,332,318,303,288,
														257,240,224,206,189,171,152,134,115,
														77,58,38,19,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0};

const unsigned short s_cuadrada_1_5A [SIZEOF_SIGNALS] = {465,465,465,465,465,465,465,465,465,465,
														465,465,465,465,465,465,465,465,465,
														465,465,465,465,465,465,465,465,465,
														465,465,465,465,465,465,465,465,465,
														465,465,465,465,465,465,465,465,465,
														465,465,465,465,465,465,465,465,465,
														465,465,465,465,465,465,465,465,465,
														465,465,465,465,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0};

const unsigned short s_triangular_1_5A [SIZEOF_SIGNALS] = {0,6,12,18,24,31,37,43,49,55,
														68,74,80,86,93,99,105,111,117,
														130,136,142,148,155,161,167,173,179,
														192,198,204,210,217,223,229,235,241,
														254,260,266,272,279,285,291,297,303,
														316,322,328,334,341,347,353,359,365,
														378,384,390,396,403,409,415,421,427,
														440,446,452,458,465,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0,
														0,0,0,0,0,0,0,0,0};

const unsigned short s_triangular_6A [SIZEOF_SIGNALS] = {0,11,23,35,47,59,71,83,95,107,
                                                           131,143,155,167,179,191,203,215,227,
                                                           251,263,275,287,299,311,323,335,347,
                                                           371,383,395,407,419,431,443,455,467,
                                                           491,503,515,527,539,551,563,575,587,
                                                           611,623,635,647,659,671,683,695,707,
                                                           731,743,755,767,779,791,803,815,827,
                                                           851,863,875,887,899,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0};


//--- FUNCIONES DEL MODULO ---//
void TreatmentManager (void)
{
    switch (treatment_state)
    {
        case TREATMENT_INIT_FIRST_TIME:
            UpdateLaserCh1(0);
            UpdateLaserCh2(0);
            UpdateLaserCh3(0);
            UpdateLaserCh4(0);

            Update_TIM3_CH1(0);
            Update_TIM3_CH2(0);
            Update_TIM3_CH3(0);
            Update_TIM3_CH4(0);

            if (AssertTreatmentParams() == resp_ok)
            {
                treatment_state = TREATMENT_STANDBY;
                ChangeLed(LED_TREATMENT_STANDBY);
            }
            break;

        case TREATMENT_STANDBY:
            break;

        case TREATMENT_START_TO_GENERATE:    //reviso una vez mas los parametros y no tener ningun error
            if ((AssertTreatmentParams() == resp_ok) && (GetErrorStatus() == ERROR_OK))
            {
#ifdef USE_SOFT_OVERCURRENT
                //cargo valor maximo de corriente para el soft_overcurrent
                soft_overcurrent_treshold = 1.2 * I_MAX;
                soft_overcurrent_index = 0;

                for (unsigned char i = 0; i < SIZEOF_OVERCURRENT_BUFF; i++)
                    soft_overcurrent_max_current_in_cycles[i] = 0;
#endif
                if (signal_to_gen.signal == CWAVE_SIGNAL)
                {
                    cwave_state = INIT_CWAVE;
                    treatment_state = TREATMENT_GENERATING_CWAVE;
                }

                if (signal_to_gen.signal == PULSED_SIGNAL)
                {
                    pulsed_state = INIT_PULSED;
                    treatment_state = TREATMENT_GENERATING_PULSED;
                }

                if (signal_to_gen.signal == MODULATED_SIGNAL)
                    treatment_state = TREATMENT_GENERATING_MODULATED;

                ChangeLed(LED_TREATMENT_GENERATING);
            }
            else
            {
                //error de parametros
                treatment_state = TREATMENT_INIT_FIRST_TIME;
                ChangeLed(LED_TREATMENT_ERROR);
            }
            break;

        case TREATMENT_GENERATING_CWAVE:
            //Cosas que dependen de las muestras
            //se la puede llamar las veces que sea necesario y entre funciones, para acelerar
            //la respuesta
            GenerateSignalCWave();

#ifdef USE_SOFT_OVERCURRENT
            //TODO: poner algun synchro con muestras para que no ejecute el filtro todo el tiempo
            //soft current overload check
            // if (MAFilter8 (soft_overcurrent_max_current_in_cycles) > soft_overcurrent_treshold)
            // {
            //     treatment_state = TREATMENT_STOPPING;
            //     SetErrorStatus(ERROR_SOFT_OVERCURRENT);
            // }
#endif
            break;

        case TREATMENT_GENERATING_PULSED:
            //Cosas que dependen de las muestras
            //se la puede llamar las veces que sea necesario y entre funciones, para acelerar
            //la respuesta
            GenerateSignalPulsed();

#ifdef USE_SOFT_OVERCURRENT
            //TODO: poner algun synchro con muestras para que no ejecute el filtro todo el tiempo
            //soft current overload check
            // if (MAFilter8 (soft_overcurrent_max_current_in_cycles) > soft_overcurrent_treshold)
            // {
            //     treatment_state = TREATMENT_STOPPING;
            //     SetErrorStatus(ERROR_SOFT_OVERCURRENT);
            // }
#endif
            break;

        case TREATMENT_GENERATING_MODULATED:
            //Cosas que dependen de las muestras
            //se la puede llamar las veces que sea necesario y entre funciones, para acelerar
            //la respuesta
            GenerateSignalCWave();

#ifdef USE_SOFT_OVERCURRENT
            //TODO: poner algun synchro con muestras para que no ejecute el filtro todo el tiempo
            //soft current overload check
            // if (MAFilter8 (soft_overcurrent_max_current_in_cycles) > soft_overcurrent_treshold)
            // {
            //     treatment_state = TREATMENT_STOPPING;
            //     SetErrorStatus(ERROR_SOFT_OVERCURRENT);
            // }
#endif
            break;
            
        case TREATMENT_STOPPING:
            Update_TIM3_CH1(0);
            Update_TIM3_CH2(0);
            Update_TIM3_CH3(0);
            Update_TIM3_CH4(0);

            UpdateLaserCh1(0);
            UpdateLaserCh2(0);
            UpdateLaserCh3(0);
            UpdateLaserCh4(0);
            
            timer_signals = 10;
            treatment_state = TREATMENT_STOPPING2;
            break;

        case TREATMENT_STOPPING2:
            if (!timer_signals)
                treatment_state = TREATMENT_INIT_FIRST_TIME;

            break;

        default:
            treatment_state = TREATMENT_INIT_FIRST_TIME;
            break;
    }
}

// void TreatmentManager_IntSpeed (void)
// {
//     switch (treatment_state)
//     {
//     case TREATMENT_INIT_FIRST_TIME:
//         HIGH_LEFT_PWM(0);
//         LOW_LEFT_PWM(0);
//         HIGH_RIGHT_PWM(0);
//         LOW_RIGHT_PWM(DUTY_ALWAYS);

//         if (GetErrorStatus() == ERROR_OK)
//         {
//             discharge_state = INIT_DISCHARGE;
//             treatment_state = TREATMENT_GENERATING;
//             LED_OFF;
//             EXTIOn();
//         }
//         break;

//     case TREATMENT_GENERATING:
//         //Cosas que dependen de las muestras
//         //se la puede llamar las veces que sea necesario y entre funciones, para acelerar
//         //la respuesta
//         GenerateSignal();

//         break;

//     case TREATMENT_STOPPING2:		//aca lo manda directamente la int
//         if (!timer_signals)
//         {
//             treatment_state = TREATMENT_INIT_FIRST_TIME;
//             EXTIOff();
//             ENABLE_TIM3;
//             LED_OFF;
//             SetErrorStatus(ERROR_FLUSH_MASK);
//         }
//         break;

//     default:
//         treatment_state = TREATMENT_INIT_FIRST_TIME;
//         break;
//     }
// }

treatment_t GetTreatmentState (void)
{
    return treatment_state;
}

resp_t StartTreatment (void)
{
    if (treatment_state == TREATMENT_STANDBY)
    {
        if ((AssertTreatmentParams() == resp_ok) && (GetErrorStatus() == ERROR_OK))
        {
            treatment_state = TREATMENT_START_TO_GENERATE;
            return resp_ok;
        }
    }
    return resp_error;
}

void StopTreatment (void)
{
    if (treatment_state != TREATMENT_STANDBY)
        treatment_state = TREATMENT_STOPPING;
}

error_t GetErrorStatus (void)
{
	error_t error = ERROR_OK;

	if (global_error & ERROR_OVERTEMP_MASK)
		error = ERROR_OVERTEMP;
	else if (global_error & ERROR_OVERCURRENT_MASK)
		error = ERROR_OVERCURRENT;
	else if (global_error & ERROR_NO_CURRENT_MASK)
		error = ERROR_NO_CURRENT;
	else if (global_error & ERROR_SOFT_OVERCURRENT_MASK)
		error = ERROR_SOFT_OVERCURRENT;

	return error;
}

// void SetErrorStatus (error_t e)
// {
//     if (e == ERROR_FLUSH_MASK)
//         global_error = 0;
//     else
//     {
//         if (e == ERROR_OVERTEMP)
//             global_error |= ERROR_OVERTEMP_MASK;
//         if (e == ERROR_OVERCURRENT)
//             global_error |= ERROR_OVERCURRENT_MASK;
//         if (e == ERROR_SOFT_OVERCURRENT)
//             global_error |= ERROR_SOFT_OVERCURRENT_MASK;
//         if (e == ERROR_NO_CURRENT)
//             global_error |= ERROR_NO_CURRENT_MASK;
//     }
// }

// //TODO: PONER UNA TRABA DE SETEOS PARANO CAMBIAR NADA CORRIENDO

resp_t SetSignalType (signal_type_t a)
{
    //TODO: despues cargar directamente los k
    if ((treatment_state != TREATMENT_INIT_FIRST_TIME) && (treatment_state != TREATMENT_STANDBY))
        return resp_error;

    if (a == CWAVE_SIGNAL)
        p_signal = (unsigned short *) s_cuadrada_1_5A;

    if (a == PULSED_SIGNAL)
        p_signal = (unsigned short *) s_triangular_1_5A;    

    if (a == MODULATED_SIGNAL)
        p_signal = (unsigned short *) s_senoidal_1_5A;

    signal_to_gen.signal = a;

    return resp_ok;
}

resp_t SetFrequency (unsigned char a)
{
    if ((a >= 0) && (a < 10))
        signal_to_gen.frequency = a;

    return resp_ok;
}

resp_t SetPowerLed (unsigned char ch, unsigned char a)
{
    if (ch == 0x0F)
    {              
        if (a > 100)
        {            
            signal_to_gen.ch1_power_led = 100;
            signal_to_gen.ch2_power_led = 100;
            signal_to_gen.ch3_power_led = 100;
            signal_to_gen.ch4_power_led = 100;
        }
        else
        {
            signal_to_gen.ch1_power_led = a;
            signal_to_gen.ch2_power_led = a;
            signal_to_gen.ch3_power_led = a;
            signal_to_gen.ch4_power_led = a;
        }
    }

    if (ch == 1)
    {              
        if (a > 100)
            signal_to_gen.ch1_power_led = 100;
        else
            signal_to_gen.ch1_power_led = a;
    }

    if (ch == 2)
    {              
        if (a > 100)
            signal_to_gen.ch2_power_led = 100;
        else
            signal_to_gen.ch2_power_led = a;
    }

    if (ch == 3)
    {              
        if (a > 100)
            signal_to_gen.ch3_power_led = 100;
        else
            signal_to_gen.ch3_power_led = a;
    }

    if (ch == 4)
    {              
        if (a > 100)
            signal_to_gen.ch4_power_led = 100;
        else
            signal_to_gen.ch4_power_led = a;
    }

    return resp_ok;
}

resp_t SetPowerLaser (unsigned char ch, unsigned char a)
{
    if (ch == 0x0F)
    {              
        if (a > 100)
        {            
            signal_to_gen.ch1_power_laser = 100;
            signal_to_gen.ch2_power_laser = 100;
            signal_to_gen.ch3_power_laser = 100;
            signal_to_gen.ch4_power_laser = 100;
        }
        else
        {
            signal_to_gen.ch1_power_laser = a;
            signal_to_gen.ch2_power_laser = a;
            signal_to_gen.ch3_power_laser = a;
            signal_to_gen.ch4_power_laser = a;
        }
    }

    if (ch == 1)
    {              
        if (a > 100)
            signal_to_gen.ch1_power_laser = 100;
        else
            signal_to_gen.ch1_power_laser = a;
    }

    if (ch == 2)
    {              
        if (a > 100)
            signal_to_gen.ch2_power_laser = 100;
        else
            signal_to_gen.ch2_power_laser = a;
    }

    if (ch == 3)
    {              
        if (a > 100)
            signal_to_gen.ch3_power_laser = 100;
        else
            signal_to_gen.ch3_power_laser = a;
    }

    if (ch == 4)
    {              
        if (a > 100)
            signal_to_gen.ch4_power_laser = 100;
        else
            signal_to_gen.ch4_power_laser = a;
    }

    return resp_ok;
}

//verifica que se cumplan con todos los parametros para poder enviar una senial coherente
resp_t AssertTreatmentParams (void)
{
    resp_t resp = resp_error;

    if ((signal_to_gen.ch1_power_led > 100) ||
        (signal_to_gen.ch2_power_led > 100) ||
        (signal_to_gen.ch3_power_led > 100) ||
        (signal_to_gen.ch4_power_led > 100) ||
        (signal_to_gen.ch1_power_laser > 100) ||
        (signal_to_gen.ch2_power_laser > 100) ||
        (signal_to_gen.ch3_power_laser > 100) ||
        (signal_to_gen.ch4_power_laser > 100))
        return resp;

    if (signal_to_gen.frequency > 9)
        return resp;

    if ((signal_to_gen.signal != CWAVE_SIGNAL) &&
        (signal_to_gen.signal != PULSED_SIGNAL) &&
        (signal_to_gen.signal != MODULATED_SIGNAL))
        return resp;

    return resp_ok;
}

void SendAllConf (void)
{
    char b [64];
    sprintf(b, "signal: %d\n", signal_to_gen.signal);
    Usart1Send(b);
    sprintf(b, "freq: %d\n", signal_to_gen.frequency);
    Usart1Send(b);
    sprintf(b, "ch led power: %d, %d, %d, %d\n",
            signal_to_gen.ch1_power_led,
            signal_to_gen.ch2_power_led,
            signal_to_gen.ch3_power_led,
            signal_to_gen.ch4_power_led);
    
    Usart1Send(b);
    sprintf(b, "ch laser power: %d, %d, %d, %d\n",
            signal_to_gen.ch1_power_laser,
            signal_to_gen.ch2_power_laser,
            signal_to_gen.ch3_power_laser,
            signal_to_gen.ch4_power_laser);

    Usart1Send(b);
    Usart1Send("\n");
}

//la llama el manager para generar las seniales CWAVE en los canales
void GenerateSignalCWave (void)
{
    unsigned short dummy;
    
    switch (cwave_state)
    {
        case (INIT_CWAVE):
            //por ahora solo laser
            dummy = signal_to_gen.ch1_power_laser * 255;
            dummy = dummy / 100;
            UpdateLaserCh1(dummy);

            dummy = signal_to_gen.ch2_power_laser * 255;
            dummy = dummy / 100;
            UpdateLaserCh2(dummy);

            dummy = signal_to_gen.ch3_power_laser * 255;
            dummy = dummy / 100;
            UpdateLaserCh3(dummy);

            dummy = signal_to_gen.ch4_power_laser * 255;
            dummy = dummy / 100;
            UpdateLaserCh4(dummy);

            cwave_state = GEN_CWAVE;
            timer_signals_gen = 1000;    //cada 1 seg reviso potencias
            break;

        case GEN_CWAVE:
            if (!timer_signals_gen)
                cwave_state = INIT_CWAVE;
            break;

        default:
            //si me llaman y estoy en cualquiera igual genero
            cwave_state = INIT_CWAVE;
            break;
            
    }
}

//la llama el manager para generar las seniales PULSED en los canales
//dependen de la freq
void GenerateSignalPulsed (void)
{
    unsigned short dummy;
    
    switch (pulsed_state)
    {
        case (INIT_PULSED):
            //por ahora solo laser
            dummy = signal_to_gen.ch1_power_laser * 255;
            dummy = dummy / 100;
            UpdateLaserCh1(dummy);

            dummy = signal_to_gen.ch2_power_laser * 255;
            dummy = dummy / 100;
            UpdateLaserCh2(dummy);

            dummy = signal_to_gen.ch3_power_laser * 255;
            dummy = dummy / 100;
            UpdateLaserCh3(dummy);

            dummy = signal_to_gen.ch4_power_laser * 255;
            dummy = dummy / 100;
            UpdateLaserCh4(dummy);

            if (signal_to_gen.frequency == 0)
                timer_signals_gen = 50;
            else
                timer_signals_gen = 1000 / (signal_to_gen.frequency * 2);

            pulsed_state = GEN_PULSED;            
            break;

        case GEN_PULSED:
            if (!timer_signals_gen)
            {
                UpdateLaserCh1(0);
                UpdateLaserCh2(0);
                UpdateLaserCh3(0);
                UpdateLaserCh4(0);
                if (signal_to_gen.frequency == 0)
                    timer_signals_gen = 50;
                else
                    timer_signals_gen = 1000 / (signal_to_gen.frequency * 2);

                pulsed_state = NO_GEN_PULSED;
            }
            break;

        case NO_GEN_PULSED:
            if (!timer_signals_gen)
                pulsed_state = INIT_PULSED;
            
            break;
        
        default:
            //si me llaman y estoy en cualquiera igual genero
            pulsed_state = INIT_PULSED;
            break;            
    }
}

//la llama el manager para generar las seniales MODULATED en los canales
//dependen de la freq
void GenerateSignalModulated (void)
{
    unsigned short dummy;
    
    switch (modulated_state)
    {
        case (INIT_PULSED):
            //por ahora solo laser
            dummy = signal_to_gen.ch1_power_laser * 255;
            dummy = dummy / 100;
            UpdateLaserCh1(dummy);

            dummy = signal_to_gen.ch2_power_laser * 255;
            dummy = dummy / 100;
            UpdateLaserCh2(dummy);

            dummy = signal_to_gen.ch3_power_laser * 255;
            dummy = dummy / 100;
            UpdateLaserCh3(dummy);

            dummy = signal_to_gen.ch4_power_laser * 255;
            dummy = dummy / 100;
            UpdateLaserCh4(dummy);

            if (signal_to_gen.frequency == 0)
                timer_signals_gen = 50;
            else
                timer_signals_gen = 1000 / (signal_to_gen.frequency * 2);

            pulsed_state = GEN_PULSED;            
            break;

        case GEN_PULSED:
            if (!timer_signals_gen)
            {
                UpdateLaserCh1((signal_to_gen.ch1_power_laser) >> 1);
                UpdateLaserCh2((signal_to_gen.ch2_power_laser) >> 1);
                UpdateLaserCh3((signal_to_gen.ch3_power_laser) >> 1);
                UpdateLaserCh4((signal_to_gen.ch4_power_laser) >> 1);
                if (signal_to_gen.frequency == 0)
                    timer_signals_gen = 50;
                else
                    timer_signals_gen = 1000 / (signal_to_gen.frequency * 2);

                pulsed_state = NO_GEN_PULSED;
            }
            break;

        case NO_GEN_PULSED:
            if (!timer_signals_gen)
                pulsed_state = INIT_PULSED;
            
            break;
        
        default:
            //si me llaman y estoy en cualquiera igual genero
            pulsed_state = INIT_PULSED;
            break;            
    }
}


//--- end of file ---//
