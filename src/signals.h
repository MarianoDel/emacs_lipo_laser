/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SIGNALS_H
#define __SIGNALS_H
#include "comm.h"		//para respuestas

//--- Exported types ---//
typedef enum {
	TREATMENT_INIT_FIRST_TIME = 0,
	TREATMENT_STANDBY,
	TREATMENT_START_TO_GENERATE,
	TREATMENT_GENERATING,
	TREATMENT_GENERATING_WITH_SYNC,
	TREATMENT_STOPPING,
	TREATMENT_STOPPING2

} treatment_t;

typedef enum {
	ERROR_OK = 0,
	ERROR_OVERCURRENT,
	ERROR_NO_CURRENT,
	ERROR_SOFT_OVERCURRENT,
	ERROR_OVERTEMP

} error_t;

typedef enum {
    CWAVE_SIGNAL = 0,
    PULSED_SIGNAL,
    MODULATED_SIGNAL

} signal_type_t;

typedef struct {
    signal_type_t signal;

    unsigned char frequency;

    unsigned char ch1_power_led;
    unsigned char ch2_power_led;
    unsigned char ch3_power_led;
    unsigned char ch4_power_led;

    unsigned char ch1_power_laser;
    unsigned char ch2_power_laser;
    unsigned char ch3_power_laser;
    unsigned char ch4_power_laser;
    
    //internals
    unsigned short kprop;
    unsigned short kinteg;
    unsigned short kderv;

} signals_struct_t;

//TIPO de descarga y estado de signal
typedef enum
{
	INIT_DISCHARGE = 0,
	NORMAL_DISCHARGE,
	TAU_DISCHARGE,
	FAST_DISCHARGE,
	STOPPED_BY_INT

} discharge_state_t;


//--- Exported constants ---//

//--- Exported macro ---//
#define SIZEOF_SIGNALS		150

#define ERROR_OVERCURRENT_MASK			0x01
#define ERROR_NO_CURRENT_MASK				0x02
#define ERROR_OVERTEMP_MASK				0x04
#define ERROR_SOFT_OVERCURRENT_MASK		0x08
#define ERROR_FLUSH_MASK					0xff

#define SIZEOF_OVERCURRENT_BUFF			8

#define FlushErrorStatus() SetErrorStatus(ERROR_FLUSH_MASK)

//--- Exported functions ---//
resp_t SetSignalType (signal_type_t);
resp_t SetFrequency (unsigned char);
resp_t SetPowerLed (unsigned char, unsigned char);
resp_t SetPowerLaser (unsigned char, unsigned char);
void GenerateSignal (void);
resp_t AssertTreatmentParams (void);
treatment_t GetTreatmentState (void);
resp_t StartTreatment (void);
void StopTreatment (void);
error_t GetErrorStatus (void);
void SetErrorStatus (error_t);
void SendAllConf (void);
void TreatmentManager (void);


#endif
//--- End ---//
//--- END OF FILE ---//
