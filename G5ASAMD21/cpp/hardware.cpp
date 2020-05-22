#include "types.h"
#include "core.h"
#include "time.h"

#include "hardware.h"
#include "options.h"

#define I2C I2C3

__align(16) T_HW::DMADESC DmaTable[12];
__align(16) T_HW::DMADESC DmaWRB[12];

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

u16 gen_period = 6249;
u16 gen_freq = 10;

static void SaveGenTime();

#define GenTCC		HW::TCC2
#define WinTC		HW::TC3
#define bTC			HW::TC4
#define mTC			HW::TC5
#define D_FIRE		(1<<16)
#define SYNC		(1<<20)
#define SPI			HW::SPI5
#define M1			(1<<9)
#define M2			(1<<8)
#define TLS			(1<<0) 

static void InitVectorTable();

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*----------------------------------------------------------------------------
  Initialize the system
 *----------------------------------------------------------------------------*/
extern "C" void SystemInit()
{
	//u32 i;
	using namespace CM0;
	using namespace HW;

	HW::PIOA->DIRSET = (1<<15)|(1<<13)|(1<<1)|(1<<2)|(1<<3)|D_FIRE|M1|M2;
	HW::PIOA->CLR((1<<15)|(1<<13)|(1<<1)|(1<<2)|(1<<3)|D_FIRE|M1|M2);

	HW::NVMCTRL->CTRLB = NVMCTRL_MANW|NVMCTRL_RWS_HALF;

	//SYSCTRL->OSC8M &= ~OSC8M_PRESC_8;

	SYSCTRL->XOSC = XOSC_ENABLE|XOSC_ONDEMAND; // RUNSTDBY|ENABLE

	//while (!SYSCTRL->PCLKSR.XOSCRDY);

	SYSCTRL->DPLLCTRLA = 0; while ((SYSCTRL->DPLLSTATUS & DPLLSTATUS_ENABLE) != 0);

	SYSCTRL->DPLLCTRLB = DPLL_REFCLK_XOSC|DPLL_DIV(7);	// 0x70010; // XOSC clock source division factor 16 = 2*(DIV+1), XOSC clock reference
	SYSCTRL->DPLLRATIO = DPLL_LDR((MCK+500000)/1000000-1)|DPLL_LDRFRAC(0);	// 47; // Loop Divider Ratio = 48, Loop Divider Ratio Fractional Part = 0

	SYSCTRL->DPLLCTRLA = DPLL_ONDEMAND|DPLL_ENABLE; 

	//while ((SYSCTRL->DPLLSTATUS & 7) != 7);

	HW::GCLK->GenDivCtrl(0, 0,	GCLK_SRC_FDPLL|GCLK_GENEN|GCLK_OE);

	HW::GCLK->GenDivCtrl(1, 16,	GCLK_SRC_XOSC|GCLK_GENEN|GCLK_OE);

	HW::GCLK->GenDivCtrl(3, 1,	GCLK_SRC_XOSC|GCLK_GENEN|GCLK_OE);

	HW::GCLK->GenDivCtrl(4, 32,	GCLK_SRC_XOSC|GCLK_GENEN|GCLK_OE);

	//HW::GCLK->GENDIV	= 0|GCLK_DIV(0);
	//HW::GCLK->GENCTRL	= 0|GCLK_GENEN|GCLK_OE|GCLK_SRC_FDPLL;

	//HW::GCLK->GENDIV	= 1|GCLK_DIV(16);
	//HW::GCLK->GENCTRL	= 1|GCLK_GENEN|GCLK_OE|GCLK_SRC_XOSC;

	//HW::GCLK->GENDIV	= 3|GCLK_DIV(1);
	//HW::GCLK->GENCTRL	= 3|GCLK_GENEN|GCLK_OE|GCLK_SRC_XOSC;

	//HW::GCLK->GENDIV	= 4|GCLK_DIV(32);
	//HW::GCLK->GENCTRL	= 4|GCLK_GENEN|GCLK_OE|GCLK_SRC_XOSC;

	HW::PIOB->PMUX[22/2] = 0x77;
	HW::PIOB->PINCFG[22] = 0x41;

	HW::DMAC->CTRL = 0;
	HW::DMAC->BASEADDR	= DmaTable;
	HW::DMAC->WRBADDR	= DmaWRB;
	HW::DMAC->CTRL = DMAC_DMAENABLE|DMAC_LVLEN0|DMAC_LVLEN1|DMAC_LVLEN2|DMAC_LVLEN3;

//	InitVectorTable();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static __irq void IntDummyHandler()
//{
//	__breakpoint(0);
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static __irq void HardFaultHandler()
//{
//	__breakpoint(0);
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static __irq void ExtDummyHandler()
//{
//	__breakpoint(0);
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static void InitVectorTable()
//{
//	for (u32 i = 0; i < ArraySize(VectorTableInt); i++)
//	{
//		VectorTableInt[i] = IntDummyHandler;
//	};
//
//	for (u32 i = 0; i < ArraySize(VectorTableExt); i++)
//	{
//		VectorTableExt[i] = ExtDummyHandler;
//	};
//
//	VectorTableInt[3] = HardFaultHandler;
//
//	CM0::SCB->VTOR = (u32)VectorTableInt;
//}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

	gen_period = (1000000UL + (gen_freq>>1)) / gen_freq - 1;

	GenTCC->PER = gen_period;
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
	static u32 pt = 0;

	u32 t = GetMilliseconds();
	u32 dt = t - pt;

	pt = t;

	if (dt != 0/* && (TCC0.CTRLB & 0x10)*/)
	{
		genWorkTimeMilliseconds += dt;

		if (genWorkTimeMilliseconds >= 60000)
		{
			genWorkTimeMinutes += 1;

			genWorkTimeMilliseconds -= 60000;

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
	//TCC0.CTRLB = TC_WGMODE_SS_gc|0x10;
	//TCC0.CTRLE = 0; 		
	//TCC0.PER = gen_period;
	//TCC0.CCA = (15 * CPU_FREQ + 32 * 1000000) / (1000000 * 64);
	//TCC0.INTCTRLA = 0;
	//TCC0.INTCTRLB = TC_CCAINTLVL_HI_gc;
	//TCC0.CTRLA = TC_CLKSEL_DIV64_gc;
	HW::PIOA->PINCFG[16] = 1;

	genTimeOut = 60000;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DisableGen()
{
	HW::PIOA->PINCFG[16] = 0;
	genTimeOut = 0;

//	TCC0.CTRLB = TC_WGMODE_SS_gc;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static __irq void GenIRQ()
{
	using namespace HW;

	PIOA->BSET(17);

	GenTCC->INTFLAG = ~0;

	cur_ts_index = 0;
	max_ts_index = windowCount;

	WinTC->INTENSET = TC_MC0;

	WinTC->CTRLA = TC_ENABLE|TC_WAVEGEN_MFRQ;
//	WinTC->CTRLBSET = TC_CMD_RETRIGGER;

	bTC->CTRLBSET = TC_CMD_RETRIGGER;
	mTC->CTRLBSET = TC_CMD_RETRIGGER;

//	if (TCC0.CTRLB & 0x10) { fireCount += 1; };

	PIOA->BCLR(17);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static __irq void WinIRQ()
{
	using namespace HW;

	PIOA->BSET(18);

	WinTC->INTFLAG = ~0;

	b_ts[cur_ts_index] += bTC->COUNT;
	m_ts[cur_ts_index] += mTC->COUNT;

	bTC->CTRLBSET = TC_CMD_RETRIGGER;
	mTC->CTRLBSET = TC_CMD_RETRIGGER;

	cur_ts_index += 1;

	if (cur_ts_index >= 64/*max_ts_index*/)
	{
		WinTC->CTRLA &= ~TC_ENABLE;
//		WinTC->CTRLBSET = TC_CMD_STOP;
	};

	PIOA->BCLR(18);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void InitGen()
{
	using namespace HW;

	HW::GCLK->CLKCTRL = GCLK_ID_TCC2_TC3|GCLK_GEN(1)|GCLK_CLKEN;
	HW::GCLK->CLKCTRL = GCLK_ID_TC4_TC5|GCLK_GEN(0)|GCLK_CLKEN;

	PM->APBCMASK |= PM_APBC_TCC2|PM_APBC_TC3|PM_APBC_TC4|PM_APBC_TC5;

	HW::PIOA->DIRSET = D_FIRE|(7<<17);;
	HW::PIOA->CLR(D_FIRE);

	HW::PIOA->PINCFG[16] = 1;
	HW::PIOA->SetPinMux(16, 4);

	HW::PIOA->PINCFG[24] = 1;
	HW::PIOA->PINCFG[25] = 1;
	HW::PIOA->SetPinMux(24, 0);
	HW::PIOA->SetPinMux(25, 0);


	bTC->CTRLA = TC_SWRST;
	mTC->CTRLA = TC_SWRST;
	while((bTC->STATUS|mTC->STATUS) & TC_SYNCBUSY);


	bTC->READREQ = TC_RCONT|0x10;
	mTC->READREQ = TC_RCONT|0x10;

	bTC->EVCTRL = TC_TCEI|TC_EVACT_COUNT;
	mTC->EVCTRL = TC_TCEI|TC_EVACT_COUNT;

	bTC->CC[0] = ~1;
	mTC->CC[0] = ~1;

	bTC->CTRLA = TC_WAVEGEN_MFRQ|TC_ENABLE;
	mTC->CTRLA = TC_WAVEGEN_MFRQ|TC_ENABLE;

	// Win timer

	WinTC->CTRLA = TC_SWRST;
	while(WinTC->STATUS & TC_SYNCBUSY);

	WinTC->READREQ = TC_RCONT|0x18;

	WinTC->CC[0] = windowTime - 1; // временнќе окно
	WinTC->CTRLA = TC_WAVEGEN_MFRQ;
	//WinTC->CTRLA = TC_ENABLE|TC_WAVEGEN_MFRQ;
	//while(WinTC->STATUS & TC_SYNCBUSY);
	//WinTC->CTRLBSET = TC_CMD_STOP;


	VectorTableExt[TC3_IRQ] = WinIRQ;
	CM0::NVIC->ICPR[0] = 1 << TC3_IRQ;
	CM0::NVIC->ISER[0] = 1 << TC3_IRQ;	

	// Gen timer

	GenTCC->CTRLA = TCC_SWRST;

	while(GenTCC->SYNCBUSY);

	VectorTableExt[TCC2_IRQ] = GenIRQ;
	CM0::NVIC->ICPR[0] = 1 << TCC2_IRQ;
	CM0::NVIC->ISER[0] = 1 << TCC2_IRQ;	


	GenTCC->WAVE = 2;
	GenTCC->PER = gen_period;
	GenTCC->CC[0] = 1;

	GenTCC->INTENCLR = ~0;
	GenTCC->INTENSET = TCC_OVF;

	GenTCC->INTFLAG = ~0;

	GenTCC->CTRLA = TCC_PRESCALER_DIV16;
	GenTCC->CTRLA = TCC_PRESCALER_DIV16|TCC_ENABLE;

	//TCC0.CTRLB = TC_WGMODE_SS_gc|0x10;
	//TCC0.CTRLE = 0; 		
	//TCC0.PER = gen_period;
	//TCC0.CCA = (15 * CPU_FREQ + 32 * 1000000) / (1000000 * 64);
	//TCC0.INTCTRLA = 0;
	//TCC0.INTCTRLB = TC_CCAINTLVL_HI_gc;
	//TCC0.CTRLA = TC_CLKSEL_DIV64_gc;

	genTimeOut = 60000;
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
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline void ManDisable()	{ HW::PIOA->CLR(M1|M2);}
inline void ManOne()		{ HW::PIOA->CLR(M1); HW::PIOA->SET(M2); }
inline void ManZero()		{ HW::PIOA->CLR(M2); HW::PIOA->SET(M1); } 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static byte manInv = 0;

#define BOUD2CLK(x) ((u32)(1000000/x+0.5))
#define ManTmr		HW::TCC0
#define ManRT		HW::TCC1
#define ManRxd()	(HW::PIOA->IN & 1)

static const u16 manboud[4] = { BOUD2CLK(20833), BOUD2CLK(41666), BOUD2CLK(62500), BOUD2CLK(83333) };//0:20833Hz, 1:41666Hz,2:62500Hz,3:83333Hz


u16 trmHalfPeriod = BOUD2CLK(20833)/2;
byte stateManTrans = 0;
static MTB *manTB = 0;
static bool trmBusy = false;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void SetTrmBoudRate(byte i)
{
	trmHalfPeriod = manboud[i&3]/2;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static u16 rcvCount = 0;
static bool rcvBusy = false;
byte stateManRcvr = 0;

const u16 rcvPeriod = BOUD2CLK(20833);
//const u16 rcvHalfPeriod = rcvPeriod/2;
//const u16 rcvHalfPeriodMin = rcvHalfPeriod*0.75;
//const u16 rcvHalfPeriodMax = rcvHalfPeriod*1.25;
//const u16 rcvQuartPeriod = rcvPeriod/4;
//const u16 rcvSyncPulse = rcvPeriod * 1.5;
//const u16 rcvSyncPulseMin = rcvSyncPulse * 0.8;
//const u16 rcvSyncPulseMax = rcvSyncPulse * 1.2;
//const u16 rcvSyncHalf = rcvSyncPulseMax + rcvHalfPeriod;
//const u16 rcvPeriodMin = rcvPeriod * 0.8;
//const u16 rcvPeriodMax = rcvPeriod * 1.2;

//static byte rcvSyncState = 0;
//static byte rcvDataState = 0;

static u16* rcvManPtr = 0;
static u16 rcvManCount = 0;


static u16 rcvManLen = 0;
static u32 rcvManPrevTime = 0;


static MRB *manRB = 0;


static __irq void WaitManCmdSync();
//static __irq void WaitManDataSync();
//static __irq void ManRcvSync();


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//bool trmStartCmd = false;
//bool trmStartData = false;

//u32 icount = 0;
//static byte ib;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// 11110, 01001, 10100, 10101, 01010, 01011, 01110, 01111, 10010, 10011, 10110, 10111, 11010, 11011, 11100, 11101

byte tbl_4B5B[16] = { 0x1E, 0x09, 0x14, 0x15, 0x0A, 0x0B, 0x0E, 0x0F, 0x12, 0x13, 0x16, 0x17, 0x1A, 0x1B, 0x1C, 0x1D };

inline u32 Encode_4B5B(u16 v) { return tbl_4B5B[v&15]|(tbl_4B5B[(v>>4)&15]<<5)|(tbl_4B5B[(v>>8)&15]<<10)|(tbl_4B5B[(v>>12)&15]<<15); }


// 0 - L1
// 1 - H1
// 2 - H2
// 3 - L2

// L2 H2 H1 L1
// Z - 1111, P - 1100, Z - 1111, N - 0011

u16 mltArr[4] = { M1, 0, M2, 0 };
byte mltSeq = 0;

#define MltTmr HW::TCC0

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline void MltOff()	{ HW::PIOA->CLR(M1|M2);} // 0110  
inline void MltZ()		{ HW::PIOA->CLR(M1|M2); mltSeq = 0; } // 1111
inline void MltNext()	{ HW::PIOA->CLR(M1|M2); HW::PIOA->SET(mltArr[(mltSeq++)&3]); }



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

byte stateMLT3 = 0;
static MTB *mltTB = 0;
static bool mltBusy = false;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static __irq void MLT3_TrmIRQ()
{
	static u32 tw = 0;
//	static u32 pw = 0;
	static byte nrz = 0;
//	static byte pnrz = 0;
	static u16 count = 0;
//	static byte i = 0;
	static const u16 *data = 0;
	static u16 len = 0;


	switch (stateMLT3)
	{
		case 0:	// Idle; 

			MltZ();
		
			data = mltTB->data;
			len = mltTB->len;

//			pw = 0;
			tw = Encode_4B5B(*data);

			data++;
			len--;

			nrz = 0;

			count = 20;

			stateMLT3 = 1;

			break;

		case 1: // Start data

			if (tw & 0x80000)
			{
				nrz ^= 1;
//				MltNext();
			};

			if (nrz != 0)
			{
				MltNext();
			};

			count--;
//			pw = tw;
			tw <<= 1;

			if (count == 0)
			{
				if (len > 0)
				{
					tw = Encode_4B5B(*data);

					data++;
					len--;
					count = 20;
				}
				else
				{
					stateMLT3++;
				};
			};

			break;

		case 2:	

			MltOff();
			stateMLT3 = 0;

			ManTmr->CTRLA = 0;
			ManTmr->INTENCLR = ~0;

			mltTB->ready = true;
			mltBusy = false;

			break;


	}; // 	switch (stateManTrans)

	ManTmr->INTFLAG = ~0;//TCC_OVF;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool SendMLT3(MTB *mtb)
{
	if (mltBusy || mtb == 0 || mtb->data == 0 || mtb->len == 0)
	{
		return false;
	};

	mtb->ready = false;

	mltTB = mtb;

	stateMLT3 = 0;

	ManTmr->CTRLA = 0;

	VectorTableExt[TCC0_IRQ] = MLT3_TrmIRQ;
	CM0::NVIC->ICPR[0] = 1 << TCC0_IRQ;
	CM0::NVIC->ISER[0] = 1 << TCC0_IRQ;	

	ManTmr->PER = trmHalfPeriod-1;
	//ManTmr->EVCTRL = 1<<6;

	ManTmr->INTENCLR = ~TCC_OVF;
	ManTmr->INTENSET = TCC_OVF;

	ManTmr->INTFLAG = ~0;

	ManTmr->CTRLA = TCC_ENABLE;

	//HW::PIOB->IDR = 1;

	//VectorTableExt[HW::PID::TC1_I] = MLT3_TrmIRQ;
	//CM4::NVIC->ICPR[0] = HW::PID::TC1_M;
	//CM4::NVIC->ISER[0] = HW::PID::TC1_M;	
	//MltTmr.IER = CPCS;

	//u32 tmp = MltTmr.SR;

	//MltTmr.RC = trmHalfPeriod;

	//MltTmr.CMR = CPCTRG;
	//
	//MltTmr.CCR = CLKEN|SWTRG;

	return mltBusy = true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline u16 CheckParity(u16 x)
{
	u16 y = x ^ (x >> 1);

	y = y ^ (y >> 2);
	y = y ^ (y >> 4);
	
	return (y ^ (y >> 8))^1;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static __irq void ManTrmIRQ()
{
	static u32 tw = 0;
	static u16 count = 0;
	static byte i = 0;
	static const u16 *data = 0;
	static u16 len = 0;
	static bool cmd = false;

//	HW::PIOA->BSET(13);

	switch (stateManTrans)
	{
		case 0:	// Idle; 

			ManDisable();
		
			data = manTB->data;
			len = manTB->len;
			stateManTrans = 1;
			cmd = true;

			break;

		case 1: // Start data

			i = 3;
			tw = ((u32)(*data) << 1) | (CheckParity(*data) & 1);

			data++;
			len--;

			if (cmd) { ManZero(); }	else { ManOne(); };

			//ManOne();

			stateManTrans++;

			break;

		case 2:	// Wait data 1-st sync imp

			i--;

			if (i == 0)
			{
				stateManTrans++;

				if (cmd) { ManOne(); }	else { ManZero(); };

				//ManZero();

				cmd = false;

				i = 3;
			};

			break;

		case 3: // Wait 2-nd sync imp

			i--;

			if (i == 0)
			{
				stateManTrans++;
				count = 17;

				if (tw & 0x10000) ManZero(); else ManOne();
			};

			break;

		case 4: // 1-st half bit wait

			if (tw & 0x10000) ManOne(); else ManZero();

			count--;

			if (count == 0)
			{
				if (len > 0)
				{
					stateManTrans = 1;
				}
				else if (manTB->next != 0)
				{
					manTB->ready = true;

					manTB = manTB->next;

					len = manTB->len;
					data = manTB->data;

					stateManTrans = (data != 0 && len != 0) ? 1 : 6;
				}
				else
				{
					stateManTrans = 6;
				};
			}
			else
			{
				stateManTrans++;
			};

			break;

		case 5: // 2-nd half bit wait

			tw <<= 1;
			stateManTrans = 4;
			if (tw & 0x10000) ManZero(); else ManOne();

			break;

		case 6:

			stateManTrans++;

			break;

		case 7:

			ManDisable();
			stateManTrans = 0;

			ManTmr->CTRLA = 0;
			ManTmr->INTENCLR = ~0;

			manTB->ready = true;
			trmBusy = false;

			break;


	}; // 	switch (stateManTrans)


	ManTmr->INTFLAG = ~0;//TCC_OVF;

//	HW::PIOA->BCLR(13);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool SendManData(MTB *mtb)
{
	if (trmBusy /*|| rcvBusy*/ || mtb == 0 || mtb->data == 0 || mtb->len == 0)
	{
		return false;
	};

	mtb->ready = false;

	manTB = mtb;

	stateManTrans = 0;

	ManTmr->CTRLA = 0;

	VectorTableExt[TCC0_IRQ] = ManTrmIRQ;
	CM0::NVIC->ICPR[0] = 1 << TCC0_IRQ;
	CM0::NVIC->ISER[0] = 1 << TCC0_IRQ;	

	ManTmr->PER = trmHalfPeriod-1;
	//ManTmr->EVCTRL = 1<<6;

	ManTmr->INTENCLR = ~TCC_OVF;
	ManTmr->INTENSET = TCC_OVF;

	ManTmr->INTFLAG = ~0;

	ManTmr->CTRLA = TCC_ENABLE;

	return trmBusy = true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void InitManTransmit()
{
	using namespace HW;

	HW::GCLK->CLKCTRL = GCLK_ID_TCC0_TCC1|GCLK_GEN(1)|GCLK_CLKEN;

	PM->APBCMASK |= PM_APBC_TCC0;

	HW::PIOA->DIRSET = M1|M2;

	ManTmr->CTRLA = TCC_SWRST;

	while(ManTmr->SYNCBUSY);

	VectorTableExt[TCC0_IRQ] = ManTrmIRQ;
	CM0::NVIC->ICPR[0] = 1 << TCC0_IRQ;
	CM0::NVIC->ISER[0] = 1 << TCC0_IRQ;	

	SetTrmBoudRate(0);

	ManTmr->PER = trmHalfPeriod-1;
	//ManTmr->EVCTRL = 1<<6;

	ManTmr->INTENCLR = ~TCC_OVF;
	ManTmr->INTENSET = TCC_OVF;

	ManTmr->INTFLAG = ~0;

	//ManTmr->CTRLA = TCC_ENABLE;

	ManDisable();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void ManRcvEnd(bool ok)
{
	ManRT->INTENCLR = TCC_OVF;

	manRB->OK = ok;
	manRB->ready = true;
	manRB->len = rcvManLen;
	
	rcvBusy = false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
class Receiver
{
	private:
		u32 _number;
		u32 _length;
		u32 _data_temp;
		u32 _data;
		bool _command_temp;
		bool _command;
		bool _parity_temp;
		bool _parity;
		bool _sync;
		bool _state;

    public:

		enum status_type
		{
			STATUS_WAIT = 0,
			STATUS_SYNC,
			STATUS_HANDLE,
			STATUS_CHECK,
			STATUS_READY,
			STATUS_ERROR_LENGTH,
			STATUS_ERROR_STRUCT,
		};

		Receiver(bool parity);
		status_type Parse(u16 len);	
		u16 GetData() { return _data; }
		bool GetType() { return _command; }
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Receiver::Receiver(bool parity)
{
	_state = false;
	_number = 0;
	_length = 0;
	_data_temp = 0;
	_data = 0;
	_command_temp = false;
	_command = false;
	_parity = parity;
	_parity_temp = false;
	_sync = false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Receiver::status_type Receiver::Parse(u16 len)
{	
	_state = !_state;

	if((len <= 12) || (len > 108))
	{
//		HW::PIOB->SODR = 1<<10;

		_number = 0;
		_length = 0;
		_sync = false;

//		HW::PIOB->CODR = 1<<10;

		return STATUS_ERROR_LENGTH;
	}
	else if(len <= 36)                
	{	
		_length++;
	}
	else if(len <= 60)
	{	
		_length += 2;
	}
	else
	{
		HW::PIOA->BSET(3);

		_sync = true;
		_data_temp = 0;
		_parity_temp = _parity;
		_number = 0;
		_length = (len <= 84) ? 1 : 2;
		_command_temp = !_state; 
	};

	if(_length >= 3)
	{
		_number = 0;
		_length = 0;
		_sync = false;


		return STATUS_ERROR_STRUCT;
	};

	if(_sync)
	{
		if(_length == 2)
		{
			if(_number < 16)
			{
				_data_temp <<= 1;
				_data_temp |= _state;
				_parity_temp ^= _state;
				_number ++;
				_length = 0;
			}
		 	else
			{
				_data = _data_temp;
				_data_temp = 0;
				_command = _command_temp;
				_command_temp = false;
				_number = 0;
				_length = 0;
				_sync = false;

				if(_state != _parity_temp)
				{
					_state = !_state;
					_data = (~_data);
					_command = !_command;
				};

				return STATUS_READY;
			};
		};

		return (_number < 16) ? STATUS_HANDLE : STATUS_CHECK;
	};

	return STATUS_WAIT;
}
*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static Receiver manRcv(true);

u32 lastCaptureValue = 0;
byte manRcvState = 0;

u32 manRcvTime1 = 0;
u32 manRcvTime2 = 0;
u32 manRcvTime3 = 0;
u32 manRcvTime4 = 0;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static __irq void ManRcvIRQ()
{
	using namespace HW;

	//static byte n;
	//static u32 rw;

	static u32 _number = 0;
	static u32 _length = 0;
	static u32 _data_temp = 0;
	static u32 _data = 0;
	static bool _command_temp = false;
	static bool _command = false;
	static bool _parity_temp = false;
	//static bool _parity = false;
	static bool _sync = false;
	static bool _state = false;

	HW::PIOA->BSET(1);


	//ManRT->CC[1] += 48;
 
	u32 t = GetRTT();
	u32 len = t - manRcvTime1;
	manRcvTime1 = t;

	_state = !((bool)ManRxd());

	switch (manRcvState)
	{
		case 0: //+++++++++++++++++++++++++++++++++++++++

			if (len <= 60)
			{
				if(len <= 36)                
				{	
					_length++;
				}
				else
				{	
					_length += 2;
				};

				if(_length >= 3)
				{
					_number = 0;
					_length = 0;
					_sync = false;

			//		return STATUS_ERROR_STRUCT;
					//break;
				};
			}
			else
			{

				if(len > 108)
				{
					_number = 0;
					_length = 0;
					_sync = false;
					
					//break;
				}
				else
				{
					_sync = true;
					_data_temp = 0;
					_parity_temp = false;//_parity;
					_number = 0;
					_length = (len <= 84) ? 1 : 2;
					_command_temp = !_state; 

					rcvManPrevTime = t;
				};
			};

			if(_sync && _length == 2)
			{
				ManRT->PER = 35;
				//ManRT->COUNT = 12;
				//ManRT->CTRLBCLR = TCC_ONESHOT;
				//ManRT->CTRLBSET = TCC_CMD_RETRIGGER;

				EVSYS->CHANNEL = 0|EVSYS_GEN_EIC_EXTINT_0|EVSYS_PATH_SYNCHRONOUS|EVSYS_EDGSEL_NO_EVT_OUTPUT;

				_data_temp <<= 1;

				//u32 s = ManRxd();
				//_state = s;

//				HW::PIOA->SET(s<<3);

				_data_temp |= _state;
				_parity_temp ^= _state;
				_number ++;
				_length = 0;

				rcvManPrevTime = t;

				manRcvState++;
			}
			else
			{
				ManRT->CTRLBSET = TCC_CMD_STOP;
			};

			break;

		case 1: //+++++++++++++++++++++++++++++++++++++++

//			HW::PIOA->BSET(3);

			if(_number < 16)
			{
//				u32 s = ManRxd();

//				HW::PIOA->SET(s<<3);
				//_state = s;

				ManRT->PER = 47;

				_data_temp <<= 1;
				_data_temp |= _state;
				_parity_temp ^= _state;
				_number ++;
				_length = 0;
			}
 			else
			{
				//HW::PIOA->BSET(3);

				ManRT->PER = 11;
				ManRT->CTRLBSET = TCC_CMD_STOP;

				EVSYS->CHANNEL = 0|EVSYS_GEN_EIC_EXTINT_0|EVSYS_PATH_SYNCHRONOUS|EVSYS_EDGSEL_BOTH_EDGES;

				manRcvState = 0;

				manRcvTime1 += 24;

				_data = _data_temp;
				_data_temp = 0;
				_command = _command_temp;
				_command_temp = false;
				_number = 0;
				_length = 0;
				_sync = false;

				if(_state == _parity_temp)
				{
					_state = !_state;
					_data = (~_data);
					_command = !_command;
				};

				if (rcvManLen == 0)
				{
					if(_command)
					{
						HW::PIOA->BSET(2);

						*rcvManPtr++ = _data;//manRcv.GetData();
						rcvManLen = 1;
					};

//					HW::PIOA->BCLR(3);
				}
				else 
				{
//					HW::PIOA->BSET(3);

					if(rcvManLen < rcvManCount)
					{
						*rcvManPtr++ = _data;//manRcv.GetData();
					};

					rcvManLen += 1;	
				};
			};

			rcvManPrevTime = t;

			break;

	};






	//if(manRcv.Parse(dt) == manRcv.STATUS_READY)
	//{
	//};





	ManRT->INTFLAG = ~0;

	HW::PIOA->CLR((1<<1)|(1<<2)|(1<<3));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static __irq void ManRcvIRQ2()
{
	using namespace HW;

	static u32 _number = 0;
	static u32 _length = 0;
	static u32 _data = 0;
	static bool _command = false;
	static bool _parity_temp = false;
	const bool _parity = true;
	static bool _sync = false;
	static bool _state = false;

	HW::PIOA->BSET(1);
 
	u32 t = GetRTT();
	u32 len = t - manRcvTime1;
	manRcvTime1 = t;

	_state = !_state;

	if (len <= 60)
	{
		_length += (len <= 36) ? 1 : 2;

		if(_length >= 3)
		{
			_sync = false;
		};
	}
	else
	{
		if(len > 108)
		{
			_sync = false;
		}
		else
		{
			HW::PIOA->BSET(2);

			_sync = true;
			_data = 0;
			_parity_temp = _parity;
			_number = 0;
			_length = (len <= 84) ? 1 : 2;
			_command = !_state; 
		};
	};

	if(_sync && _length == 2)
	{
		HW::PIOA->BSET(3);

		if(_number < 16)
		{
			_data <<= 1;
			_data |= _state;
			_parity_temp ^= _state;
			_number++;
			_length = 0;
		}
	 	else
		{
			HW::PIOA->BSET(2);

			_sync = false;

			if(_state != _parity_temp)
			{
				_state = !_state;
				_data = (~_data);
				_command = !_command;
			};

			rcvManPrevTime = t;

			if (rcvManLen == 0)
			{
				if(_command)
				{
					*rcvManPtr++ = _data;
					rcvManLen = 1;
				};
			}
			else 
			{
				if(rcvManLen < rcvManCount)
				{
					*rcvManPtr++ = _data;
				};

				rcvManLen += 1;	
			};
		};
	};

	ManRT->INTFLAG = ~0;

	HW::PIOA->CLR((1<<1)|(1<<2)|(1<<3));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ManRcvUpdate()
{
	if (rcvBusy)
	{
		//bool c = false;//ManTmr.SR & CPCS;

		if (rcvManLen > 0 && (GetRTT() - manRcvTime1) > US2RT(200))
		{
//			__disable_irq();

			HW::PIOA->BSET(3);
			ManRcvEnd(true);
			HW::PIOA->BCLR(3);

//			__enable_irq();
		}
		else
		{
			manRB->len = rcvManLen;
		};
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//void ManRcvStop()
//{
//	ManRcvEnd(true);
//}
//
////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void InitManRecieve()
{
	using namespace HW;

	PM->APBCMASK |= PM_APBC_TCC1;

	HW::PIOA->DIRCLR = TLS;
	HW::PIOA->CTRL |= TLS;

	HW::PIOA->PMUX[0] = 0;
	HW::PIOA->PINCFG[0] = 3;


	ManRT->CTRLA = TCC_SWRST;

	while(ManRT->SYNCBUSY);

	VectorTableExt[TCC1_IRQ] = ManRcvIRQ2;
	CM0::NVIC->ICPR[0] = 1 << TCC1_IRQ;
	CM0::NVIC->ISER[0] = 1 << TCC1_IRQ;	

	ManRT->CTRLA = 0;
	ManRT->EVCTRL = TCC_TCEI0|TCC_EVACT0_RETRIGGER;

	ManRT->PER = 11;
	ManRT->CC[0] = ~0;
	ManRT->CC[1] = ~0;

	ManRT->INTENCLR = ~0;
//	ManRT->INTENSET = TCC_OVF;

	ManRT->INTFLAG = ~0;

	ManRT->CTRLA = TCC_ENABLE;

	while(ManRT->SYNCBUSY);

	ManRT->CTRLBSET = TCC_ONESHOT;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool RcvManData(MRB *mrb)
{
	if (rcvBusy /*|| trmBusy*/ || mrb == 0 || mrb->data == 0 || mrb->maxLen == 0)
	{
		return false;
	};

	//ManDisable();

	mrb->ready = mrb->OK = false;
	mrb->len = 0;

	manRB = mrb;
	
	rcvManLen = 0;

	rcvManPtr = manRB->data;
	rcvManCount = manRB->maxLen;

	//HW::EVSYS->CHANNEL = 0|EVSYS_GEN_EIC_EXTINT_0|EVSYS_PATH_SYNCHRONOUS|EVSYS_EDGSEL_BOTH_EDGES;

	//ManRT->CTRLBSET = TCC_ONESHOT;
	//ManRT->PER = 11;

	ManRT->INTFLAG = ~0;
	ManRT->INTENSET = TCC_OVF;

	return rcvBusy = true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static byte *twi_wrPtr = 0;
static byte *twi_rdPtr = 0;
static u16 twi_wrCount = 0;
static u16 twi_rdCount = 0;
static byte *twi_wrPtr2 = 0;
static u16 twi_wrCount2 = 0;
static byte twi_adr = 0;
static DSCI2C* twi_dsc = 0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

__irq void Handler_I2C()
{
	using namespace HW;

	byte state = I2C->INTFLAG;

	if(state & I2C_SB) // Received data is available
	{
		*twi_rdPtr++ = I2C->DATA; // receive data

		twi_rdCount--;

		I2C->CTRLB |= (twi_rdCount > 0) ? I2C_CMD_2 : I2C_CMD_STOP; 
	}
	else if(state & I2C_MB) // Data can be transmitted 
	{
		if (twi_wrCount > 0)
		{
			I2C->DATA = *twi_wrPtr++;

			twi_wrCount--;

			if(twi_wrCount == 0 && twi_wrCount2 != 0)
			{
				twi_wrPtr = twi_wrPtr2;
				twi_wrCount = twi_wrCount2;
				twi_wrCount2 = 0;
			};
		}
		else if (twi_rdCount > 0)
		{
			I2C->ADDR = (twi_adr << 1) | 1;
		}
		else
		{
			I2C->CTRLB |= I2C_CMD_STOP;
		};
	}
	else
	{
		twi_rdCount = 0;
		twi_wrCount = 0;

		I2C->CTRLB |= I2C_CMD_STOP;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool Write_I2C(DSCI2C *d)
{
	using namespace HW;

	if (twi_dsc != 0 || d == 0) { return false; };
	if ((d->wdata == 0 || d->wlen == 0) && (d->rdata == 0 || d->rlen == 0)) { return false; }

//	smask = 1<<13;
	twi_dsc = d;

	VectorTableExt[SERCOM3_IRQ] = Handler_I2C;
	CM0::NVIC->ICPR[0] = 1 << SERCOM3_IRQ;
	CM0::NVIC->ISER[0] = 1 << SERCOM3_IRQ;

	twi_dsc->ready = false;

	twi_wrPtr = (byte*)twi_dsc->wdata;	
	twi_rdPtr = (byte*)twi_dsc->rdata;	
	twi_wrPtr2 = (byte*)twi_dsc->wdata2;	
	twi_wrCount = twi_dsc->wlen;
	twi_wrCount2 = twi_dsc->wlen2;
	twi_rdCount = twi_dsc->rlen;
	twi_adr = twi_dsc->adr;

	if (twi_wrPtr2 == 0) twi_wrCount2 = 0;

	__disable_irq();

	I2C->STATUS.BUSSTATE = BUSSTATE_IDLE;

	I2C->INTFLAG = ~0;
	I2C->INTENSET = I2C_MB|I2C_SB;

	I2C->ADDR = (twi_dsc->adr << 1) | ((twi_wrCount == 0) ? 1 : 0);
		
	__enable_irq();

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool Check_I2C_ready()
{
	if (twi_dsc == 0)
	{ 
		return true; 
	}
	else if (HW::I2C->STATUS.BUSSTATE == BUSSTATE_IDLE)
	{
		twi_dsc->ready = true;
		twi_dsc = 0;

		return true;
	}
	else
	{
		return false;
	};
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool Init_I2C()
{
	using namespace HW;

	HW::GCLK->CLKCTRL = GCLK_ID_SERCOM3_CORE|GCLK_GEN(3)|GCLK_CLKEN;
	HW::GCLK->CLKCTRL = GCLK_ID_SERCOMX_SLOW|GCLK_GEN(2)|GCLK_CLKEN;

	PM->APBCMASK |= PM_APBC_SERCOM3;

	HW::PIOA->PMUX[22/2] = 0x22;
	HW::PIOA->PINCFG[22] = 0x01;
	HW::PIOA->PINCFG[23] = 0x01;

	HW::PIOA->DIRSET = (1<<22)|(1<<23);

	I2C->CTRLA = I2C_SWRST;

	while(I2C->SYNCBUSY);

	I2C->CTRLA = SERCOM_MODE_I2C_MASTER;

	I2C->CTRLA = SERCOM_MODE_I2C_MASTER|I2C_INACTOUT_205US|I2C_SPEED_SM;
	I2C->CTRLB = 0;
	I2C->BAUD = 0x0F0C;

	I2C->CTRLA |= I2C_ENABLE;

	while(I2C->SYNCBUSY);

	I2C->STATUS = 0;
	I2C->STATUS.BUSSTATE = BUSSTATE_IDLE;

	VectorTableExt[SERCOM3_IRQ] = Handler_I2C;
	CM0::NVIC->ICPR[0] = 1 << SERCOM3_IRQ;
	CM0::NVIC->ISER[0] = 1 << SERCOM3_IRQ;

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void EEPROM_Read(byte *src, byte *dst, u16 size)
{
	//while(size)
	//{
	//	*dst = eeprom_read_byte(src);
	//	dst ++;
	//	src ++;
	//	size --;
	//}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void EEPROM_Write(byte *src, byte *dst, u16 size)
{
	//while(size)
	//{
	//	eeprom_write_byte(src, *dst);
	//	dst ++;
	//	src ++;
	//	size --;
	//}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool EEPROM_Verify(byte *src, byte *dst, u16 size)
{
	bool verify = true;	
	//while((size) && (verify))
	//{
	//	if(eeprom_read_byte(src) != *dst) verify = false;
	//	dst ++;
	//	src ++;
	//	size --;
	//}
	return verify;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void AD5312_Set(byte channel, u16 dac)
{
	//u16 data = ((u16)channel << 15) | (dac << 2);

	//PORTC.OUTCLR = PIN4_bm; //PIN7_bm;

	//for(byte i = 0; i < 16; i++)
	//{
	//	if(data & 0x8000)
	//	{
	//		PORTC.OUTSET = PIN5_bm;
	//	}
	//	else
	//	{
	//		PORTC.OUTCLR = PIN5_bm;
	//	};

	//	PORTC.OUTSET = PIN7_bm; //PIN6_bm;

	//	data <<= 1;

	//	PORTC.OUTCLR = PIN7_bm; //PIN6_bm;
	//}

	//PORTC.OUTSET = PIN4_bm; //PIN7_bm;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void Init_AD5312()
{
	using namespace HW;
	
	HW::GCLK->CLKCTRL = GCLK_ID_SERCOM5_CORE|GCLK_GEN(0)|GCLK_CLKEN;
	HW::GCLK->CLKCTRL = GCLK_ID_SERCOMX_SLOW|GCLK_GEN(2)|GCLK_CLKEN;

	PM->APBCMASK |= PM_APBC_SERCOM5;

	HW::PIOB->PMUX[16/2] = 0x22;

	HW::PIOB->PINCFG[16] = 0x01;
	HW::PIOB->PINCFG[17] = 0x01;

	HW::PIOA->SetPinMux(20, 2);
	HW::PIOA->PINCFG[20] = 1;

	HW::PIOA->DIRSET = SYNC;

	SPI->CTRLA = SERCOM_MODE_SPI_MASTER|SPI_CPHA|SPI_CPOL|SPI_FORM_SPI|SPI_DIPO(0)|SPI_DOPO(0);

	SPI->CTRLB = SPI_CHSIZE_8BIT|SPI_MSSEN;
	SPI->BAUD = 23;

	SPI->CTRLA |= SPI_ENABLE;

	while(SPI->SYNCBUSY);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void Update_AD5312()
{
	static TM32 tm;

	if (tm.Check(10))
	{
		if (SPI->INTFLAG & SPI_DRE)
		{
			SPI->DATA = 0x55;
		};
	};

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void InitHardware()
{
	using namespace HW;

	HW::PIOA->BSET(13);

//	InitVectorTable();

	HW::GCLK->CLKCTRL = GCLK_ID_EIC|GCLK_GEN(1)|GCLK_CLKEN;
	HW::GCLK->CLKCTRL = GCLK_ID_EVSYS_0|GCLK_GEN(1)|GCLK_CLKEN; // Manchester reciever
	HW::GCLK->CLKCTRL = GCLK_ID_EVSYS_1|GCLK_GEN(0)|GCLK_CLKEN; // NNKB
	HW::GCLK->CLKCTRL = GCLK_ID_EVSYS_2|GCLK_GEN(0)|GCLK_CLKEN; // NNKM

	PM->APBAMASK |= PM_APBA_EIC;
	PM->APBCMASK |= PM_APBC_EVSYS;
	
	EIC->CTRL = EIC_SWRST;

	while(EIC->STATUS);

	EIC->EVCTRL = EIC_EXTINT0|EIC_EXTINT12|EIC_EXTINT13;
	EIC->CONFIG[0] = EIC_FILTEN|EIC_SENSE_LOW;
	EIC->CONFIG[1] = (EIC_SENSE_HIGH<<16)|(EIC_SENSE_HIGH<<20);
	EIC->INTENCLR = EIC_EXTINT0|EIC_EXTINT12|EIC_EXTINT13;
	EIC->CTRL = EIC_ENABLE;

	EVSYS->USER = EVSYS_USER_CHANNEL(0)|EVSYS_USER_TCC1_EV_0;
	EVSYS->CHANNEL = 0|EVSYS_GEN_EIC_EXTINT_0|EVSYS_PATH_SYNCHRONOUS|EVSYS_EDGSEL_BOTH_EDGES;

	EVSYS->USER = EVSYS_USER_CHANNEL(1)|EVSYS_USER_TC4_EVU;
	EVSYS->CHANNEL = 1|EVSYS_GEN_EIC_EXTINT_12|EVSYS_PATH_SYNCHRONOUS|EVSYS_EDGSEL_BOTH_EDGES;

	EVSYS->USER = EVSYS_USER_CHANNEL(2)|EVSYS_USER_TC5_EVU;
	EVSYS->CHANNEL = 2|EVSYS_GEN_EIC_EXTINT_13|EVSYS_PATH_SYNCHRONOUS|EVSYS_EDGSEL_BOTH_EDGES;

	Init_time();
	Init_I2C();
	InitManTransmit();
	InitManRecieve();
	InitGen();
	Init_AD5312();

#ifndef _DEBUG

	//if ((WDT->CTRL & 0x82) == 0)
	//{
	//	WDT->CONFIG = 0x0B; // 16384 clock cycles
	//	WDT->CTRL = 0x82; // Always on, enabled
	//};

	//ResetWDT();

#endif

	HW::PIOA->BCLR(13);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void UpdateHardware()
{
	static byte i = 0;

//	static Deb db(false, 20);

	#define CALL(p) case (__LINE__-S): p; break;

	enum C { S = (__LINE__+3) };
	switch(i++)
	{
		CALL( UpdateGenTime()		);
		CALL( Update_AD5312();		);
		CALL( 						);
		CALL( 						);
		CALL( 						);
	};

	i = (i > (__LINE__-S-3)) ? 0 : i;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
u16 GetCRC(const void *data, u32 len)
{
	union { u32 *pd; u16 *pw; u8 *pb; };

	pb = (byte*)data;

//	byte * const p = (byte*)HW::CRC->B;

	HW::CRC->MODE = 0x15;
	HW::CRC->SEED = 0xFFFF;

	u32 dl = len>>2;
	u32 wl = (len&3)>>1;
	u32 bl = (len&1);

	for ( ; dl > 0; dl--) 
	{
		HW::CRC->D = *(pd++);
	};

	for ( ; wl > 0; wl--) 
	{
		HW::CRC->W = *(pw++);
	};

	for ( ; bl > 0; bl--) 
	{
		HW::CRC->B = *(pb++);
	};

	//for ( ; len > 0; len--) 
	//{
	//	HW::CRC->B = *(pb++);
	//};

	return HW::CRC->SUM;
}*/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
