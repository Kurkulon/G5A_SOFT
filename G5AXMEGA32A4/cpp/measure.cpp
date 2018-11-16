#include "common.h"
#include "measure.h"
#include "hardware.h"
#include "options.h"
#include "telemetry.h"

/************************************************************************/
u16 measure_nnkb_timer = 0;
u16 measure_nnkm_timer = 0;

u16 Measure_Get_NNKB()
{
	u16 temp = NNKB_Timer_Get();
	u16 result = (u16)(temp - measure_nnkb_timer);
	measure_nnkb_timer = temp;
	return result;
}

u16 Measure_Get_NNKM()
{
	u16 temp = NNKM_Timer_Get();
	u16 result = (u16)(temp - measure_nnkm_timer);
	measure_nnkm_timer = temp;
	return result;
}

/************************************************************************/
//u16 measure_gk1_timer = 0;
//u16 measure_gk2_timer = 0;

//u16 Measure_Get_GK1()
//{
//	u16 temp = GK1_Timer_Get();
//	u16 result = (u16)(temp - measure_gk1_timer);
//	measure_gk1_timer = temp;
//	return result;
//}

//u16 Measure_Get_GK2()
//{
//	u16 temp = GK2_Timer_Get();
//	u16 result = (u16)(temp - measure_gk2_timer);
//	measure_gk2_timer = temp;
//	return result;
//}


/************************************************************************/
i16 measure_temperature_in = 0;
i32 measure_temperature_in_sum = 0;
u16 measure_temperature_in_count = 0;

void Measure_Idle_Temperature_In()
{
	if(ADC_Idle(ADC_CH_TEMPERATURE))
	{
		measure_temperature_in = ADC_Get(ADC_CH_TEMPERATURE);	
		if(measure_temperature_in_count != 0xFFFF)
		{
			measure_temperature_in_sum += measure_temperature_in;
			measure_temperature_in_count ++;	
		}
	}
	ADC_Start(ADC_CH_TEMPERATURE);
}

i16 Measure_Get_Current_Temperature_In()
{
	return Temperature_Sensor_Calibrate(measure_temperature_in);
}

i16 Measure_Get_Temperature_In()
{
	u16 t = (u16)(measure_temperature_in_sum / measure_temperature_in_count);
	measure_temperature_in_sum = 0;
	measure_temperature_in_count = 0;
	return Temperature_Sensor_Calibrate(t);
}
/************************************************************************/
//static const i16 measure_correction_table[2][33] = {{-100,-50,0,50,100,150,200,250,300,350,400,450,500,550,600,650,700,750,800,850,900,950,1000,1050,1100,1150,1200,1250,1300,1350,1400,1450,1500},
//{3787,3762,3731,3693,3647,3591,3524,3446,3357,3255,3141,3016,2880,2736,2585,2429,2271,2112,1956,1804,1657,1518,1386,1263,1149,1044,947,859,779,706,640,581,527}};
//
//i16 measure_correction = 0;
//i32 measure_correction_sum = 0;
//u16 measure_correction_count = 0;

//void Measure_Idle_Correction()
//{
//	if(ADC_Idle(ADC_CH_CORRECTION))
//	{
//		measure_correction = ADC_Get(ADC_CH_CORRECTION);	
//		if(measure_correction_count != 0xFFFF)
//		{
//			measure_correction_sum += measure_correction;
//			measure_correction_count ++;	
//		}
//	}
//	ADC_Start(ADC_CH_CORRECTION);
//}

//i16 Measure_Calibrate_Correction(i16 t)
//{
//	byte z = (sizeof(measure_correction_table)/sizeof(i16))/2;
//	if ( t > measure_correction_table[1][0]) return measure_correction_table[0][0];
//	if ( t < measure_correction_table[1][z - 1]) return measure_correction_table[0][z - 1];
// 	byte i = 0;
// 	while ((i < z - 2) && (t < measure_correction_table[1][i+1])) i++;
//	return (i16)(measure_correction_table[0][i+1] - ((measure_correction_table[0][i+1] - measure_correction_table[0][i])*(measure_correction_table[1][i+1] - t))/(measure_correction_table[1][i+1] - measure_correction_table[1][i])) ;
//}

//inline i16 Measure_Get_Current_Correction()
//{
//	return Measure_Calibrate_Correction(measure_correction);
//}

//i16 Measure_Get_Correction()
//{
//	i16 c = (i16)(measure_correction_sum / measure_correction_count);
//	measure_correction_sum = 0;
//	measure_correction_count = 0;
//	return Measure_Calibrate_Correction(c);
//}

/************************************************************************/
void Measure_Init()
{
}

void Measure_Idle()
{
	Measure_Idle_Temperature_In();	
//	Measure_Idle_Correction();
}

