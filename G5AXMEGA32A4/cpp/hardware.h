#ifndef _HARDWARE_H
#define _HARDWARE_H

#define CPU_FREQ		16000000ul
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

inline void ADC_Start(byte ch) {	ADCA.CTRLA |= (1 << (ch + ADC_CH0START_bp)); }	//стартуем АЦП

inline u16 ADC_Get(byte ch) { extern u16 adc_result[8]; return adc_result[ch]; }


extern i16 Temperature_Sensor_Calibrate(i16 t);
extern void EEPROM_Read(byte *src, byte *dst, u16 size);
extern void EEPROM_Write(byte *src, byte *dst, u16 size);
extern bool EEPROM_Verify(byte *src, byte *dst, u16 size);

#define RXD_TIMER_FREQ			(CPU_FREQ_PERIPHERY4 / 64)
#define TXD_TIMER_FREQ			(CPU_FREQ_PERIPHERY4 / 8)
#define RXD_TXD_INTERRUPT()		ISR(TCE0_CCA_vect)
#define RXD_TXD_CLEAR_INTERRUPT()	TCE0.INTFLAGS = TC0_CCAIF_bm
#define RXD_TXD_INTERRUPT_FLAG()	TCE0.INTFLAGS & TC0_CCAIF_bm
extern byte RXD_Timer_Get();
extern byte RXD_Get();
extern void RXD_Start();
extern void RXD_Stop();
extern void TXD_Set(byte p);
extern void TXD_Start();
extern void TXD_Stop();
#define TXD_OUT_LO()		{ PORTA.OUTTGL = ((PORTA.OUT & (PIN3_bm | PIN4_bm)) ^ (PIN3_bm)); }
#define TXD_OUT_HI()		{ PORTA.OUTTGL = ((PORTA.OUT & (PIN3_bm | PIN4_bm)) ^ (PIN4_bm)); } 
#define TXD_OUT_ME()		{ PORTA.OUTTGL = ((PORTA.OUT & (PIN3_bm | PIN4_bm)) ^ (0)); }
#define TXD_OUT_OFF()		{ PORTA.OUTCLR = PIN3_bm | PIN4_bm; }

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

extern u16 GetRTC();

inline u16 GetGenFreq() { extern byte gen_freq; return gen_freq; }
inline u16 GetWindowCount() { extern u16 windowCount; return windowCount; }
inline u16 GetWindowTime() {extern u16 windowTime; return windowTime; }
inline u16 GetGenWorkTime() {extern u16 genWorkTimeMinutes; return genWorkTimeMinutes; }
inline u16 GetFireCount() {extern u16 fireCount; cli(); u16 v = fireCount; fireCount = 0; sei(); return v; }

extern u32 m_ts[];
extern u32 b_ts[];

//#define US2RT(x) ((x*32768+500000)/1000000)
#define MS2RT(x) ((x+4)/8)


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct TM16
{
	u16 pt;

	TM16() : pt(0) {}
	bool Check(u16 v) {u16 t = GetRTC(); if ((t - pt) >= v) { pt = t; return true; } else { return false; }; }
	void Reset() { pt = GetRTC(); }
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



#endif
