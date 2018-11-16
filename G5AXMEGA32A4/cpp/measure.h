#ifndef _MEASURE_H
#define _MEASURE_H

extern void Measure_Init();
extern void Measure_Idle();

extern i16 Measure_Get_Temperature_In();
extern i16 Measure_Get_Current_Temperature_In();

extern u16 Measure_Get_NNKB();
extern u16 Measure_Get_NNKM();

//extern u16 Measure_Get_GK1();
//extern u16 Measure_Get_GK2();

//extern i16 Measure_Get_Correction();
//extern i16 Measure_Get_Current_Correction();

#endif
