#ifndef HARDWARE_H__23_12_2013__11_37
#define HARDWARE_H__23_12_2013__11_37

#include "types.h"
#include "core.h"

#ifdef WIN32
#include <windows.h>
#endif

#define CPU_FREQ			MCK
#define CPU_FREQ_PERIPHERY 	CPU_FREQ * 1
#define CPU_FREQ_PERIPHERY2	CPU_FREQ * 1
#define CPU_FREQ_PERIPHERY4 CPU_FREQ * 1

#define US2CLK(t) (((1000000 * t) + CPU_FREQ / 2) / CPU_FREQ)

#define ADC_DISCRETENESS_POW	12
#define ADC_DISCRETENESS        (1u << ADC_DISCRETENESS_POW)
#define ADC_MAX			(i16)(0xFFFF >> (16 - (ADC_DISCRETENESS_POW - 1)))
#define ADC_MID			0
#define ADC_MIN			(i16)(0xFFFF << (ADC_DISCRETENESS_POW - 1))
#define ADC_CH_TEMPERATURE	0
#define ADC_CH_CORRECTION	1

extern void Hardware_Init();

extern u16 NNKB_Timer_Get();
extern u16 NNKM_Timer_Get();

//extern u16 GK1_Timer_Get();
//extern u16 GK2_Timer_Get();

extern void Test_Pin_Set();
extern void Test_Pin_Clr();

extern bool ADC_Idle(byte ch);

//inline void ADC_Start(byte ch) {	ADCA.CTRLA |= (1 << (ch + ADC_CH0START_bp)); }	//стартуем АЦП

inline u16 ADC_Get(byte ch) { extern u16 adc_result[8]; return adc_result[ch]; }


extern i16 Temperature_Sensor_Calibrate(i16 t);
extern void EEPROM_Read(byte *src, byte *dst, u16 size);
extern void EEPROM_Write(byte *src, byte *dst, u16 size);
extern bool EEPROM_Verify(byte *src, byte *dst, u16 size);

#define RXD_TIMER_FREQ			(CPU_FREQ_PERIPHERY4 / 16)
#define TXD_TIMER_FREQ			(CPU_FREQ_PERIPHERY4 / 16)
//#define RXD_TXD_INTERRUPT()		ISR(TCE0_CCA_vect)
#define RXD_TXD_CLEAR_INTERRUPT()	TCE0.INTFLAGS = TC0_CCAIF_bm
#define RXD_TXD_INTERRUPT_FLAG()	TCE0.INTFLAGS & TC0_CCAIF_bm
extern u32 RXD_Timer_Get();
extern u32 RXD_Get();
extern void RXD_Start();
extern void RXD_Stop();
extern void TXD_Set(byte p);
extern void TXD_Start();
extern void TXD_Stop();
#define TXD_OUT_LO()		{ HW::PIOA->BCLR(9); HW::PIOA->BSET(8); }
#define TXD_OUT_HI()		{ HW::PIOA->BCLR(8); HW::PIOA->BSET(9); } 
#define TXD_OUT_ME()		{ HW::PIOA->CLR(3<<8); asd }
#define TXD_OUT_OFF()		{ HW::PIOA->CLR(3<<8); }

#define AD5312_CHANNEL_LEVEL_B		0
#define AD5312_CHANNEL_LEVEL_M		1
extern void AD5312_Set(byte channel, u16 dac);

extern void SetGenFreq(u16 freq);
extern void SetWindowCount(u16 wc);
extern void SetWindowTime(u16 wt);
extern void ResetGenWorkTime();

extern void UpdateHardware();
extern void EnableGen();
extern void DisableGen();

//extern u16 GetRTC();

inline u16 GetGenFreq() { extern u16 gen_freq; return gen_freq; }
inline u16 GetWindowCount() { extern u16 windowCount; return windowCount; }
inline u16 GetWindowTime() {extern u16 windowTime; return windowTime; }
inline u16 GetGenWorkTime() {extern u16 genWorkTimeMinutes; return genWorkTimeMinutes; }
inline u16 GetFireCount() {extern u16 fireCount; __disable_irq(); u16 v = fireCount; fireCount = 0; __enable_irq(); return v; }

extern u32 m_ts[];
extern u32 b_ts[];

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct DSCI2C
{
	void*	wdata;
	void*	rdata;
	void*	wdata2;
	u16		wlen;
	u16		wlen2;
	u16		rlen;
	byte	adr;
	bool	ready;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern bool Write_I2C(DSCI2C *d);
inline bool Read_I2C(DSCI2C *d) { return Write_I2C(d); }
extern bool Check_I2C_ready();

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct MRB
{
	bool	ready;
	bool	OK;
	u16		len;
	u16		maxLen;
	u16		*data;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct MTB
{
	MTB*		next;
	const u16	*data;
	u16			len;
	bool		ready;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern bool RcvManData(MRB *mrb);
extern bool SendManData(MTB *mtb);
extern void SetTrmBoudRate(byte i);
extern void ManRcvUpdate();
//extern void ManRcvStop();

extern bool SendMLT3(MTB *mtb);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern void InitHardware();
extern void UpdateHardware();

#endif // HARDWARE_H__23_12_2013__11_37
