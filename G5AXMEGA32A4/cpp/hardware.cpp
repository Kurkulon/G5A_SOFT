#include "common.h"
#include "hardware.h"
#include "telemetry.h"
#include "control.h"
#include "manchester.h"
#include "options.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u32 m_ts[256];
u32 b_ts[256];

u16 cur_ts_index = 0;
u16 max_ts_index = 64;
u16 windowCount = 64;
u16 windowTime = 64;
u16 genWorkTimeMinutes = 0;
u16 genWorkTimeMilliseconds = 0;
u16 genTimeOut = 0;
u16 fireCount = 0;

u16 gen_period = CPU_FREQ/64/10;
byte gen_freq = 10;

static void SaveGenTime();

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void SetWindowCount(u16 wc)
{
	if (wc < 2)
	{
		wc = 2;
	}
	else if (wc > ArraySize(m_ts))
	{
		wc = ArraySize(m_ts);
	};

	options.win_count = windowCount = wc;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void SetWindowTime(u16 wt)
{
	if (wt < 2)
	{
		wt = 2;
	}
	else if (wt > 2048)
	{
		wt = 2048;
	};

	options.win_time = windowTime = wt;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void SetGenFreq(u16 freq)
{
	if (freq < 4)
	{
		freq = 4;
	}
	else if (freq > 50)
	{
		freq = 50;
	};

	options.gen_freq = gen_freq = freq;

	gen_period = (CPU_FREQ/64) / gen_freq;

	TCC0.PER = gen_period;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ResetGenWorkTime()
{
	genWorkTimeMinutes = 0;
	genWorkTimeMilliseconds = 0;

	SaveGenTime();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void UpdateGenTime()
{
	static u16 pt = 0;

	u16 t = GetRTC();
	u16 dt = t - pt;

	pt = t;

	if (dt != 0 && (TCC0.CTRLB & 0x10))
	{
		genWorkTimeMilliseconds += dt;

		if (genWorkTimeMilliseconds >= MS2RT(60000))
		{
			genWorkTimeMinutes += 1;

			genWorkTimeMilliseconds -= MS2RT(60000);

			SaveGenTime();
		};

		if (genTimeOut < dt) 
		{
			DisableGen();
		}
		else
		{
			genTimeOut -= dt;
		};
	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void EnableGen()
{
	TCC0.CTRLB = TC_WGMODE_SS_gc|0x10;
	TCC0.CTRLE = 0; 		
	TCC0.PER = gen_period;
	TCC0.CCA = (15 * CPU_FREQ + 32 * 1000000) / (1000000 * 64);
	TCC0.INTCTRLA = 0;
	TCC0.INTCTRLB = TC_CCAINTLVL_HI_gc;
	TCC0.CTRLA = TC_CLKSEL_DIV64_gc;

	genTimeOut = MS2RT(60000);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DisableGen()
{
	genTimeOut = 0;

	TCC0.CTRLB = TC_WGMODE_SS_gc;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void SaveGenTime()
{
	while(!EEPROM_Verify((byte *)32, (byte *)&genWorkTimeMinutes, sizeof(genWorkTimeMinutes)))
	{
		EEPROM_Write((byte *)32, (byte *)&genWorkTimeMinutes, sizeof(genWorkTimeMinutes));
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void LoadGenTime()
{
	EEPROM_Read((byte *)32, (byte *)&genWorkTimeMinutes, sizeof(genWorkTimeMinutes));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// From Application Note AVR1003

void CCPWrite( volatile byte * address, byte value ) 
{
    	byte volatile saved_sreg = SREG;
	volatile byte * tmpAddr = address;
	RAMPZ = 0;
	asm volatile(
		"movw r30,  %0"	      "\n\t"
		"ldi  r16,  %2"	      "\n\t"
		"out   %3, r16"	      "\n\t"
		"st     Z,  %1"       "\n\t"
		:
		: "r" (tmpAddr), "r" (value), "M" (CCP_IOREG_gc), "i" (&CCP)
		: "r16", "r30", "r31"
		);
	SREG = saved_sreg;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Hardware_Init()
{
	cli();

	// Power reduction: Stop unused peripherals 
    	
	PR.PRGEN = PR_AES_bm | PR_EBI_bm /*| PR_RTC_bm | PR_EVSYS_bm*/ | PR_DMA_bm;      
	PR.PRPA  = PR_DAC_bm /*| PR_ADC_bm | PR_AC_bm*/;
	PR.PRPB  = PR_DAC_bm | PR_ADC_bm | PR_AC_bm;
	PR.PRPC  = PR_TWI_bm | PR_USART1_bm | PR_USART0_bm | PR_HIRES_bm/* | PR_TC1_bm | PR_TC0_bm*/;
	PR.PRPD  = PR_TWI_bm | PR_USART1_bm | PR_USART0_bm | PR_HIRES_bm/* | PR_TC1_bm | PR_TC0_bm*/;
	PR.PRPE  = PR_TWI_bm | PR_USART1_bm | PR_USART0_bm | PR_HIRES_bm | PR_TC1_bm/* | PR_TC0_bm*/;
	PR.PRPF  = PR_TWI_bm | PR_USART1_bm | PR_USART0_bm | PR_HIRES_bm | PR_TC1_bm | PR_TC0_bm;

	// Clock Settings

	OSC.XOSCCTRL = OSC_XOSCSEL_EXTCLK_gc; 
	OSC.CTRL = OSC_XOSCEN_bm;        // Enable External Oscillator
	while(!(OSC.STATUS & OSC_XOSCRDY_bm));  // wait until stable

	CCPWrite(&CLK.PSCTRL, CLK_PSADIV_1_gc | CLK_PSBCDIV_1_1_gc);    
    CCPWrite(&CLK.CTRL, CLK_SCLKSEL_XOSC_gc); 

	// init PORT
	PORTC.PIN0CTRL = PORT_ISC0_bm;
	PORTC.PIN4CTRL = PORT_ISC0_bm;

	PORTA.DIR = PIN3_bm | PIN4_bm | PIN2_bm;
	PORTB.DIR = PIN0_bm | PIN2_bm;
	PORTC.DIR = PIN0_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;
	PORTD.DIR = PIN0_bm;
	PORTE.DIR = PIN0_bm;

	PORTC.OUTSET = PIN4_bm;

//	PORTA.PIN7CTRL = 0;

	// init AC

	ACA.AC0CTRL = AC_INTMODE_BOTHEDGES_gc | AC_INTLVL_OFF_gc | AC_HYSMODE_LARGE_gc;
	ACA.AC0MUXCTRL = AC_MUXPOS_PIN1_gc | AC_MUXNEG_PIN0_gc /*AC_MUXNEG_SCALER_gc*/;	
    ACA.CTRLA = AC_AC0OUT_bm;
	ACA.CTRLB = 14;
	ACA.AC0CTRL |= AC_ENABLE_bm;

	// init TCC0, TCC1 for NNKM. NNKB

	TCC0.CTRLA = TC_CLKSEL_OFF_gc;      // timer off
	TCC0.CTRLB = TC_WGMODE_SS_gc;
	TCC0.CTRLE = 0; 		
	TCC0.CNT = 0;
	TCC0.PER = gen_period;
	TCC0.CCA = (15 * CPU_FREQ + 32 * 1000000) / (1000000 * 64);
	TCC0.INTCTRLA = 0;
	TCC0.INTCTRLB = TC_CCAINTLVL_HI_gc;
	TCC0.CTRLA = TC_CLKSEL_DIV64_gc;      //
	TCC0.INTFLAGS = TC0_CCAIF_bm;
	
	TCC1.CTRLA = TC_CLKSEL_OFF_gc;      // timer off
	TCC1.CTRLB = TC_WGMODE_NORMAL_gc;
	TCC1.CTRLE = 0; 		
	TCC1.CNT = 0;
	//TCC1.CTRLA = TC_CLKSEL_EVCH3_gc;      //

	// init TCD0 for D_FIRE

	TCD0.CTRLA = TC_CLKSEL_OFF_gc;      // timer off
	TCD0.CTRLB = TC_WGMODE_NORMAL_gc; // CCAEN
	TCD0.CTRLE = 0; 		
	TCD0.CNT = 0;
	TCD0.CTRLA = TC_CLKSEL_EVCH2_gc;      //



	TCD1.CTRLA = TC_CLKSEL_OFF_gc;      // timer off
	TCD1.CTRLB = TC_WGMODE_NORMAL_gc;
	TCD1.CTRLE = 0; 		
	TCD1.CNT = 0;
	TCD1.CTRLA = TC_CLKSEL_EVCH3_gc;      //

	// init TCD1 capture for AC EVENT telemetry rxd and for  compare for telemetry txd 

	TCE0.CTRLA = TC_CLKSEL_OFF_gc;      // timer off
	TCE0.CTRLB = 0;
	TCE0.CTRLC = 0;
	TCE0.CTRLD = 0;
	TCE0.CTRLE = TC0_BYTEM_bm; 		// 8- bit mode
	TCE0.CNT = 0;
	TCE0.INTCTRLA = 0;
	TCE0.INTCTRLB = 0;
       	
	// init ADC
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
	ADCA.CALL = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0));
	ADCA.CALH = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1));
    NVM_CMD = NVM_CMD_NO_OPERATION_gc;

	ADCA.CTRLA = ADC_FLUSH_bm;
	ADCA.CTRLB = ADC_RESOLUTION_12BIT_gc;
	ADCA.REFCTRL = ADC_REFSEL_INT1V_gc | ADC_TEMPREF_bm; // было внутреннее 1В
	ADCA.EVCTRL = 0;
	ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;
	ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_INTERNAL_gc; 
	ADCA.CH0.MUXCTRL = ADC_CH_MUXINT_TEMP_gc;
	ADCA.CH0.INTCTRL = 0;
	ADCA.CH1.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_SINGLEENDED_gc; 
	ADCA.CH1.MUXCTRL = ADC_CH_MUXPOS_PIN5_gc;
	ADCA.CH1.INTCTRL = 0;
	ADCA.CTRLA |= ADC_ENABLE_bm;

	// init EVENT of ACA

	EVSYS.CH0MUX = EVSYS_CHMUX_ACA_CH0_gc;
	EVSYS.CH0CTRL = EVSYS_DIGFILT_8SAMPLES_gc;

	EVSYS.CH2MUX = EVSYS_CHMUX_PORTD_PIN2_gc;
	EVSYS.CH2CTRL = EVSYS_DIGFILT_1SAMPLE_gc;

	EVSYS.CH3MUX = EVSYS_CHMUX_PORTD_PIN3_gc;
	EVSYS.CH3CTRL = EVSYS_DIGFILT_1SAMPLE_gc;

	EVSYS.CH4MUX = EVSYS_CHMUX_PORTC_PIN0_gc;
	EVSYS.CH4CTRL = EVSYS_DIGFILT_1SAMPLE_gc;

	EVSYS.CH5MUX = EVSYS_CHMUX_PORTC_PIN4_gc;
	EVSYS.CH5CTRL = EVSYS_DIGFILT_1SAMPLE_gc;

	// init Interrupt

	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm; 

	CLK.RTCCTRL = CLK_RTCSRC_ULP_gc | CLK_RTCEN_bm;
	RTC.CTRL = RTC_PRESCALER_DIV8_gc;
	RTC.PER = 0;

	LoadGenTime();

	sei();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ISR(TCC0_CCA_vect)
{
	PORTD.OUTSET = PIN0_bm;

	cur_ts_index = 0;
	max_ts_index = windowCount;

	TCC1.CCA = (windowTime << 1) - 1; // временнОе окно

	TCC1.INTCTRLB = TC_CCAINTLVL_HI_gc;
	TCC1.CTRLB = TC_WGMODE_FRQ_gc;
	TCC1.CTRLA = TC_CLKSEL_DIV8_gc;

	TCC1.CTRLFSET = TC_CMD_RESTART_gc;

	TCD0.CTRLFSET = TC_CMD_RESTART_gc;
	TCD1.CTRLFSET = TC_CMD_RESTART_gc;

//	TCC0.INTFLAGS = TC0_CCAIF_bm;

	if (TCC0.CTRLB & 0x10) { fireCount += 1; };

	PORTD.OUTCLR = PIN0_bm;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ISR(TCC1_CCA_vect)
{
	PORTE.OUTSET = PIN0_bm;

//	TCC1.INTFLAGS = TC0_CCAIF_bm;

	b_ts[cur_ts_index] += TCD0.CNT;
	m_ts[cur_ts_index] += TCD1.CNT;

	TCD0.CTRLFSET = TC_CMD_RESTART_gc;
	TCD1.CTRLFSET = TC_CMD_RESTART_gc;

	cur_ts_index += 1;

	if (cur_ts_index >= max_ts_index)
	{
		TCC1.CTRLA = 0;
	};


	PORTE.OUTCLR = PIN0_bm;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 GetRTC()
{
	cli();

	while (RTC.STATUS != 0);

	u16 t = RTC.CNT;

	sei();

	return t;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//ISR(RTC_OVF_vect)
//{
//
//
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline void Test_Pin_Set()
{
	PORTB.OUTSET = PIN2_bm;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline void Test_Pin_Clr()
{
	PORTB.OUTCLR = PIN2_bm;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 NNKB_Timer_Get()
{
	/* алгоритм умного чтения 16-бит реализован компилятором. смотри asm*/
	u16 temp;
	cli();
	temp = TCC0.CNT;
	sei();
	return temp;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 NNKM_Timer_Get()
{
	/* алгоритм умного чтения 16-бит реализован компилятором. смотри asm*/
	u16 temp;
	cli();
	temp = TCC1.CNT;
	sei();
	return temp;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//u16 GK1_Timer_Get()
//{
//	/* алгоритм умного чтения 16-бит реализован компилятором. смотри asm*/
//	u16 temp;
//	cli();
//	temp = TCD0.CNT;
//	sei();
//	return temp;
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//u16 GK2_Timer_Get()
//{
//	/* алгоритм умного чтения 16-бит реализован компилятором. смотри asm*/
//	u16 temp;
//	cli();
//	temp = TCD1.CNT;
//	sei();
//	return temp;
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

volatile byte TCE0_CCABUF_temp;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

byte RXD_Timer_Get()
{
	return TCE0.CNTL;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

byte RXD_Get()
{
	byte temp = (byte)(TCE0.CCABUF - TCE0_CCABUF_temp);	
	TCE0_CCABUF_temp = TCE0.CCABUF;
	return temp;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void RXD_Start()
{
	TCE0.CTRLB = TC0_CCAEN_bm | TC_WGMODE_NORMAL_gc;
	TCE0.CTRLD = TC_EVACT_CAPT_gc | TC_EVSEL_CH0_gc;
	TCE0.INTCTRLB = TC_CCAINTLVL_MED_gc;
	TCE0.CTRLA = TC_CLKSEL_DIV64_gc;      // 4mks
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void RXD_Stop()
{
	TCE0.CTRLA = TC_CLKSEL_OFF_gc;      // timer off
	TCE0.CTRLB = 0;
	TCE0.CTRLD = 0;
	TCE0.CNT = 0;
	TCE0.INTCTRLB = 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void TXD_Set(byte p)
{
	TCE0.CCABUF = (p / 2) - 1;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void TXD_Start()
{
	TCE0.CTRLB = TC_WGMODE_FRQ_gc;
	TCE0.CTRLD = 0;
	TCE0.INTCTRLB = TC_CCAINTLVL_MED_gc;
	TCE0.CTRLA = TC_CLKSEL_DIV8_gc;      // 0.5mks
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void TXD_Stop()
{
	TCE0.CTRLA = TC_CLKSEL_OFF_gc;      // timer off
	TCE0.CTRLB = 0;
	TCE0.CTRLD = 0;
	TCE0.INTCTRLB = 0;
	TCE0.CNT = 0;
	TXD_OUT_OFF();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 adc_result[8];

bool ADC_Idle(byte ch)
{
	if(ADCA.INTFLAGS & (1 << (ch + ADC_CH0IF_bp)))
	{
		adc_result[ch] = *((u16 *)((u16)(&ADCA.CH0RES) + (ch * sizeof(u16))));
		ADCA.INTFLAGS = (1 << (ch + ADC_CH0IF_bp));
		return true;
	}
        return false;
}

//u16 ADC_Get(byte ch)
//{
//       return adc_result[ch];
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

i16 Temperature_Sensor_Calibrate(i16 t)
{
	u16 c;
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
	c = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE0));
	c |= ((u16)(pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE1))) << 8);
        NVM_CMD = NVM_CMD_NO_OPERATION_gc;
	return (((i32)t * (273 + 85) * 10)/c) - 2730;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void EEPROM_Read(byte *src, byte *dst, u16 size)
{
	while(size)
	{
		*dst = eeprom_read_byte(src);
		dst ++;
		src ++;
		size --;
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void EEPROM_Write(byte *src, byte *dst, u16 size)
{
	while(size)
	{
		eeprom_write_byte(src, *dst);
		dst ++;
		src ++;
		size --;
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool EEPROM_Verify(byte *src, byte *dst, u16 size)
{
	bool verify = true;	
	while((size) && (verify))
	{
		if(eeprom_read_byte(src) != *dst) verify = false;
		dst ++;
		src ++;
		size --;
	}
	return verify;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void AD5312_Set(byte channel, u16 dac)
{
	u16 data = ((u16)channel << 15) | (dac << 2);

	PORTC.OUTCLR = PIN4_bm; //PIN7_bm;

	for(byte i = 0; i < 16; i++)
	{
		if(data & 0x8000)
		{
			PORTC.OUTSET = PIN5_bm;
		}
		else
		{
			PORTC.OUTCLR = PIN5_bm;
		};

		PORTC.OUTSET = PIN7_bm; //PIN6_bm;

		data <<= 1;

		PORTC.OUTCLR = PIN7_bm; //PIN6_bm;
	}

	PORTC.OUTSET = PIN4_bm; //PIN7_bm;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void UpdateHardware()
{
	UpdateGenTime(); 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


