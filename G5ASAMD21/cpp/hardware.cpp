#include "types.h"
#include "core.h"
#include "time.h"

#include "hardware.h"
#include "options.h"
#include "list.h"
#include "PointerCRC.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define GEN_MCK			0
#define GEN_1M			1
#define GEN_32K			2
#define GEN_16M			3
#define GEN_500K		4

#define	B_DMACH			0
#define	M_DMACH			1
#define	I2C_DMACH		2
//#define	COM3_DMACH		3
//#define	SPI_DMACH_TX	4
//#define	SPI_DMACH_RX	5
//#define	DSP_DMACH		30
//#define	CRC_DMACH		31

#define I2C				HW::I2C3
#define PIO_I2C			HW::PIOA 
#define PIN_SDA			22 
#define PIN_SCL			23 
#define SDA				(1<<PIN_SDA) 
#define SCL				(1<<PIN_SCL) 
#define I2C_TRIGSRC_RX	DMCH_TRIGSRC_SERCOM3_RX
#define I2C_TRIGSRC_TX	DMCH_TRIGSRC_SERCOM3_TX

__align(16) T_HW::DMADESC 		DmaTable[12];
__align(16) T_HW::DMADESC 		DmaWRB[12];

#define SPI							HW::SPI5
#define PIO_SPI						HW::PIOB
#define PIO_SYNC					HW::PIOA

#define PIN_SCLK					17
#define PIN_MOSI					16 
#define PIN_SYNC					20 

#define SCLK						(1<<PIN_SCLK) 
#define MOSI						(1<<PIN_MOSI) 
#define SYNC						(1<<PIN_SYNC) 

#define SPI_IRQ						SERCOM5_IRQ

#define EVENT_MANR_1				0
#define EVENT_MANR_2				1
#define EVENT_DNNKB					2
#define EVENT_DNNKM					3
#define EVENT_WIN					4



#define ManTT						HW::TCC0
#define ManRT						HW::TCC1
#define GenTCC						HW::TCC2

#define WinTC						HW::TC3
#define bTC							HW::TC4
#define mTC							HW::TC5
#define ManIT						HW::TC6
//#define 							HW::TC7

#define MAN_DIV						16
#define MAN_PRESC					TC_PRESCALER_DIV16
#define MT(v)						((v)*MCK_MHz/MAN_DIV)
#define BAUD2CLK(x)					(((MCK+MAN_DIV/2)/MAN_DIV+x/2)/x)
#define US2MT(v)					((v)*MCK_MHz/MAN_DIV)

#define MANT_IRQ					TCC0_IRQ
#define MANT_IRQ_2					TCC0_IRQ

#define MANR_IRQ					TCC1_IRQ
	
#define MANR_EXTINT					0
#define MANIT_EVSYS_CHANNEL			(EVENT_MANR_1 | EVSYS_GEN_EIC_EXTINT_0 | EVSYS_PATH_SYNCHRONOUS | EVSYS_EDGSEL_BOTH_EDGES)
#define MANIT_EVSYS_USER			(EVSYS_USER_TC6_EVU | EVSYS_USER_CHANNEL(EVENT_MANR_1))

#define MANR_EVSYS_CHANNEL			(EVENT_MANR_2 | EVSYS_GEN_TC6_OVF | EVSYS_PATH_ASYNCHRONOUS | EVSYS_EDGSEL_RISING_EDGE)
#define MANR_EVSYS_USER1			(EVSYS_USER_TCC1_EV_0 | EVSYS_USER_CHANNEL(EVENT_MANR_2))
#define MANR_EVSYS_USER2			(EVSYS_USER_TCC1_MC_0 | EVSYS_USER_CHANNEL(EVENT_MANR_2))

#define ManT_SET_PR(v)				{ ManTT->PERB = (v); }
#define ManT_SET_CR(v)				{ ManTT->CCB[0] = (v); ManTT->CCB[1] = (v); }

#define PIO_MANCH					HW::PIOA
#define PIN_M1						8 
#define PIN_M2						9 
#define PIN_TLS						0
#define M1							(1<<PIN_M1)
#define M2							(1<<PIN_M2)
#define TLS							(1<<PIN_TLS)

#define Pin_ManRcvIRQ_Set()			HW::PIOA->BSET(1)
#define Pin_ManRcvIRQ_Clr()			HW::PIOA->BCLR(1)

#define Pin_ManTrmIRQ_Set()			HW::PIOA->BSET(10)		
#define Pin_ManTrmIRQ_Clr()			HW::PIOA->BCLR(10)		

#define Pin_ManRcvSync_Set()		HW::PIOB->BSET(3)		
#define Pin_ManRcvSync_Clr()		HW::PIOB->BCLR(3)		

#define PIO_D_FIRE					HW::PIOA
#define PIO_DNNK					HW::PIOA
#define PIN_D_FIRE					16
#define PIN_DNNKB					24
#define PIN_DNNKM					25
#define D_FIRE						(1<<PIN_D_FIRE)
#define DNNKB						(1<<PIN_DNNKB)
#define DNNKM						(1<<PIN_DNNKM)

#define WIN_IRQ						DMAC_IRQ
#define GEN_IRQ						TCC2_IRQ

#define DNNKB_EXTINT				12
#define DNNKM_EXTINT				13

#define DNNKB_EVSYS_GEN_EIC_EXTINT	EVSYS_GEN_EIC_EXTINT_12
#define DNNKM_EVSYS_GEN_EIC_EXTINT	EVSYS_GEN_EIC_EXTINT_13

#define DNNKB_EVSYS_CHANNEL			(EVENT_DNNKB|DNNKB_EVSYS_GEN_EIC_EXTINT|EVSYS_PATH_ASYNCHRONOUS|EVSYS_EDGSEL_RISING_EDGE)
#define DNNKB_EVSYS_USER			(EVSYS_USER_CHANNEL(EVENT_DNNKB)|EVSYS_USER_TC4_EVU)

#define DNNKM_EVSYS_CHANNEL			(EVENT_DNNKM|DNNKM_EVSYS_GEN_EIC_EXTINT|EVSYS_PATH_ASYNCHRONOUS|EVSYS_EDGSEL_RISING_EDGE)
#define DNNKM_EVSYS_USER			(EVSYS_USER_CHANNEL(EVENT_DNNKM)|EVSYS_USER_TC5_EVU)

#define GEN_DIV						256
#define GEN_PRESC					TCC_PRESCALER_DIV256
#define WIN_DIV						16
#define WIN_PRESC					TC_PRESCALER_DIV16

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*----------------------------------------------------------------------------
  Initialize the system
 *----------------------------------------------------------------------------*/
extern "C" void SystemInit()
{
	//u32 i;
	using namespace CM0;
	using namespace HW;

	HW::PIOA->CLR(PA15|PA13|PA01|PA02|PA03|D_FIRE|M1|M2);
	HW::PIOA->DIRSET = PA15|PA13|PA01|PA02|PA03|D_FIRE|M1|M2;

	HW::PIOA->BSET(15);

	HW::SYSCTRL->BOD33 = 0;
	HW::SYSCTRL->BOD33 = BOD33_HYST | BOD33_ACTION_INTERRUPT | BOD33_LEVEL(44); // Vbod = 1.5 + 34mV * BOD33_LEVEL
	HW::SYSCTRL->BOD33 |= BOD33_ENABLE;

	while ((HW::SYSCTRL->PCLKSR & (PCLKSR_BOD33RDY|PCLKSR_B33SRDY)) != (PCLKSR_BOD33RDY|PCLKSR_B33SRDY));
	while ((HW::SYSCTRL->PCLKSR & PCLKSR_BOD33DET) != 0);

	HW::SYSCTRL->BOD33 = 0;
	HW::SYSCTRL->BOD33 = BOD33_HYST | BOD33_ACTION_RESET | BOD33_LEVEL(44); // Vbod = 1.5 + 34mV * BOD33_LEVEL
	HW::SYSCTRL->BOD33 |= BOD33_ENABLE;

	HW::PIOA->BCLR(15);

	HW::NVMCTRL->CTRLB = NVMCTRL_MANW|NVMCTRL_RWS_HALF;

	SYSCTRL->XOSC = XOSC_ENABLE|XOSC_ONDEMAND; // RUNSTDBY|ENABLE

	SYSCTRL->DPLLCTRLA = 0; while ((SYSCTRL->DPLLSTATUS & DPLLSTATUS_ENABLE) != 0);

	HW::PIOA->BSET(15);

	SYSCTRL->DPLLCTRLB = DPLL_REFCLK_XOSC|DPLL_DIV(7);						// XOSC clock source division factor 16 = 2*(DIV+1), XOSC clock reference
	SYSCTRL->DPLLRATIO = DPLL_LDR((MCK+500000)/1000000-1)|DPLL_LDRFRAC(0);	// Loop Divider Ratio = 48, Loop Divider Ratio Fractional Part = 0

	SYSCTRL->DPLLCTRLA = DPLL_ONDEMAND|DPLL_ENABLE; 

	HW::GCLK->GenDivCtrl(GEN_MCK,	0,	GCLK_SRC_FDPLL		|	GCLK_GENEN	| GCLK_OE);
	HW::GCLK->GenDivCtrl(GEN_1M,	16,	GCLK_SRC_XOSC		|	GCLK_GENEN	| GCLK_OE);
	HW::GCLK->GenDivCtrl(GEN_32K,	0,	GCLK_SRC_OSCULP32K	|	GCLK_GENEN	| GCLK_OE);
	HW::GCLK->GenDivCtrl(GEN_16M,	1,	GCLK_SRC_XOSC		|	GCLK_GENEN	| GCLK_OE);
	HW::GCLK->GenDivCtrl(GEN_500K,	10,	GCLK_SRC_XOSC		|	GCLK_GENEN	| GCLK_OE);

	HW::PIOA->BCLR(15);

	HW::PIOB->SetWRCONFIG(PB22|PB23, PORT_PMUX_H | PORT_WRPINCFG | PORT_PMUXEN | PORT_WRPMUX);
	HW::PIOB->PINCFG[22] = PINGFG_DRVSTR|PINGFG_PMUXEN;
	HW::PIOB->PINCFG[23] = PINGFG_DRVSTR|PINGFG_PMUXEN;

	HW::DMAC->CTRL = 0;
	HW::DMAC->BASEADDR	= DmaTable;
	HW::DMAC->WRBADDR	= DmaWRB;
	HW::DMAC->CTRL = DMAC_DMAENABLE|DMAC_LVLEN0|DMAC_LVLEN1|DMAC_LVLEN2|DMAC_LVLEN3;

	HW::PIOA->BSET(15);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//u32 m_ts[256];
//u32 b_ts[256];

u16 cur_ts_index = 0;
u16 max_ts_index = 64;
u16 windowCount = 64;
u16 windowTime = 64;
u16 genWorkTimeMinutes = 0;
u16 genWorkTimeMilliseconds = 0;
u16 genTimeOut = 0;
u16 fireCount = 0;

u16 gen_period = 4687;
u16 gen_freq = 10;

byte saveGenCount = 0;

WINDSC windsc_arr[4];

List<WINDSC>	readyWinList;
List<WINDSC>	freeWinList;

WINDSC *curWinDsc = 0;

static void SaveGenTime();
static void PrepareWin();

u16 temp = 0;

//static void* eepromData = 0;
//static u16 eepromWriteLen = 0;
//static u16 eepromReadLen = 0;
//static u16 eepromStartAdr = 0;

inline u16 ReverseWord(u16 v) { __asm	{ rev16 v, v };	return v; }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline void SaveGenTime() { saveGenCount = 1; }

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void InitWinLsit()
{
	for (u32 i = 0; i < ArraySize(windsc_arr); i++) freeWinList.Add(&windsc_arr[i]);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

WINDSC* AllocWinDsc()
{
	return freeWinList.Get();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void FreeWinDsc(WINDSC* d)
{
	freeWinList.Add(d);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

WINDSC* GetReadyWinDsc()
{
	return readyWinList.Get();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void SetWindowCount(u16 wc)
{
	if (wc < 2)
	{
		wc = 2;
	}
	else if (wc > WINDOW_SIZE)
	{
		wc = WINDOW_SIZE;
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

	gen_period = ((MCK / GEN_DIV) + (gen_freq>>1)) / gen_freq - 1;

	GenTCC->PERB = gen_period;
	GenTCC->CTRLBSET = TCC_CMD_UPDATE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ResetGenWorkTime()
{
	genWorkTimeMinutes = 0;
	genWorkTimeMilliseconds = 0;

	SaveGenTime();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void EnableGen()
{
	PIO_D_FIRE->PINCFG[PIN_D_FIRE] = PINGFG_PMUXEN;

	genTimeOut = 60000;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DisableGen()
{
	PIO_D_FIRE->PINCFG[PIN_D_FIRE] = 0;
	genTimeOut = 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline bool CheckGenEnabled()
{
	return PIO_D_FIRE->PINCFG[PIN_D_FIRE] & PINGFG_PMUXEN;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void LoadGenTime()
{
	static DSCI2C dsc;
	static u16 romAdr = 0;
	
	byte buf[sizeof(genWorkTimeMinutes)*2+8];

	bool loadVarsOk = false;

	romAdr = ReverseWord(FRAM_I2C_GENTIME_ADR);

	dsc.wdata = &romAdr;
	dsc.wlen = sizeof(romAdr);
	dsc.wdata2 = 0;
	dsc.wlen2 = 0;
	dsc.rdata = buf;
	dsc.rlen = sizeof(buf);
	dsc.adr = 0x50;

	if (I2C_AddRequest(&dsc))
	{
		while (!dsc.ready) { Upadte_I2C_DMA(); };
	};

	PointerCRC p(buf);

	for (byte i = 0; i < 2; i++)
	{
		p.CRC.w = 0xFFFF;
		p.ReadArrayB(&genWorkTimeMinutes, sizeof(genWorkTimeMinutes));
		p.ReadW();

		if (p.CRC.w == 0) { loadVarsOk = true; break; };
	};

	if (!loadVarsOk)
	{
		genWorkTimeMinutes = 0;

		saveGenCount = 2;
	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void UpdateSaveGenTime()
{
	static DSCI2C dsc;

	static u16 romAdr = 0;
	static byte buf[sizeof(genWorkTimeMinutes) * 2 + 8];

	static byte i = 0;
	static TM32 tm;

	PointerCRC p(buf);

	switch (i)
	{
		case 0:

			if (saveGenCount > 0)
			{
				saveGenCount--;
				i++;
			};

			break;

		case 1:

			for (byte j = 0; j < 2; j++)
			{
				p.CRC.w = 0xFFFF;
				p.WriteArrayB(&genWorkTimeMinutes, sizeof(genWorkTimeMinutes));
				p.WriteW(p.CRC.w);
			};

			romAdr = ReverseWord(FRAM_I2C_GENTIME_ADR);

			dsc.wdata = &romAdr;
			dsc.wlen = sizeof(romAdr);
			dsc.wdata2 = buf;
			dsc.wlen2 = p.b-buf;
			dsc.rdata = 0;
			dsc.rlen = 0;
			dsc.adr = 0x50;

			tm.Reset();

			I2C_AddRequest(&dsc);
				
			i++;

			break;

		case 2:

			if (dsc.ready || tm.Check(100))
			{
				i = 0;
			};

			break;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void UpdateGenTime()
{
	static u32 pt = 0;

	u32 t = GetMilliseconds();
	u32 dt = t - pt;

	pt = t;

	if (dt != 0)
	{
		if (CheckGenEnabled())
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

		UpdateSaveGenTime();
	};

	if (curWinDsc == 0)	PrepareWin();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void PrepareWin()
{
	using namespace HW;

	curWinDsc = AllocWinDsc();

	if (curWinDsc == 0) return;

	WinTC->CTRLA = TC_SWRST;
	while(WinTC->STATUS & TC_SYNCBUSY);

	//WinTC->READREQ = TC_RCONT|0x18;

	WinTC->CC16[0] = windowTime*(MCK_MHz/WIN_DIV) - 1; // временнќе окно
	WinTC->CC16[1] = 0; // временнќе окно
	WinTC->EVCTRL = TC_OVFEO;
	WinTC->CTRLA = WIN_PRESC|TC_WAVEGEN_MFRQ;

	curWinDsc->fireCount = 0;
	curWinDsc->genWorkTime = genWorkTimeMinutes;
	curWinDsc->temp = 0;
	curWinDsc->winCount = windowCount;

	DMAC->CHID = B_DMACH;

	DmaTable[B_DMACH].SRCADDR = &bTC->COUNT16;
	DmaTable[B_DMACH].DSTADDR = curWinDsc->b_data + windowCount;
	DmaTable[B_DMACH].DESCADDR = 0;
	DmaTable[B_DMACH].BTCNT = windowCount;
	DmaTable[B_DMACH].BTCTRL = DMDSC_VALID|DMDSC_BEATSIZE_HWORD|DMDSC_DSTINC|DMDSC_BLOCKACT_INT;

	DMAC->CHCTRLB = DMCH_TRIGACT_BEAT|DMCH_TRIGSRC_TC3_OVF;
	DMAC->CHINTENSET = DMCH_TCMPL;
	DMAC->CHCTRLA = DMCH_ENABLE;

	DMAC->CHID = M_DMACH;

	DmaTable[M_DMACH].SRCADDR = &mTC->COUNT16;
	DmaTable[M_DMACH].DSTADDR = curWinDsc->m_data + windowCount;
	DmaTable[M_DMACH].DESCADDR = 0;
	DmaTable[M_DMACH].BTCNT = windowCount;
	DmaTable[M_DMACH].BTCTRL = DMDSC_VALID|DMDSC_BEATSIZE_HWORD|DMDSC_DSTINC|DMDSC_BLOCKACT_INT;

	DMAC->CHCTRLB = DMCH_TRIGACT_BEAT|DMCH_TRIGSRC_TC3_OVF;
	DMAC->CHINTENSET = DMCH_TCMPL;
	DMAC->CHCTRLA = DMCH_ENABLE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static __irq void GenIRQ()
{
	using namespace HW;

	PIOA->BSET(17);

	GenTCC->INTFLAG = ~0;

	if (curWinDsc != 0)
	{
		WinTC->CTRLA = TC_ENABLE|TC_PRESCALER_DIV16|TC_WAVEGEN_MFRQ;

		bTC->CTRLBSET = TC_CMD_RETRIGGER;
		mTC->CTRLBSET = TC_CMD_RETRIGGER;

		if (CheckGenEnabled()) { fireCount += 1; };
	};

	PIOA->BCLR(17);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static __irq void Win_DMA_IRQ()
{
	using namespace HW;

	PIOA->BSET(19);

	u32 stat = DMAC->INTSTATUS;

	if (stat & (1<<B_DMACH))
	{
		DMAC->CHID = B_DMACH;
		DMAC->CHINTFLAG = ~0;
	};

	if (stat & (1<<M_DMACH))
	{
		DMAC->CHID = M_DMACH;
		DMAC->CHINTFLAG = ~0;
	};

	if ((DMAC->BUSYCH & ((1<<B_DMACH)|(1<<M_DMACH))) == 0)
	{
		WinTC->CTRLA &= ~TC_ENABLE;

		readyWinList.Add(curWinDsc); curWinDsc = 0;

		//freeWinList.Add(curWinDsc); curWinDsc = 0;
	};

	PIOA->BCLR(19);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void InitGen()
{
	using namespace HW;

	HW::GCLK->CLKCTRL = GCLK_ID_TCC2_TC3				|	GCLK_GEN(GEN_MCK)	|	GCLK_CLKEN;
	HW::GCLK->CLKCTRL = GCLK_ID_TC4_TC5					|	GCLK_GEN(GEN_MCK)	|	GCLK_CLKEN;
	HW::GCLK->CLKCTRL = (GCLK_ID_EVSYS_0 + EVENT_DNNKB)	|	GCLK_GEN(GEN_MCK)	|	GCLK_CLKEN; // NNKB
	HW::GCLK->CLKCTRL = (GCLK_ID_EVSYS_0 + EVENT_DNNKB)	|	GCLK_GEN(GEN_MCK)	|	GCLK_CLKEN; // NNKM

	PM->APBCMASK |= PM_APBC_TCC2|PM_APBC_TC3|PM_APBC_TC4|PM_APBC_TC5;
	PM->APBCMASK |= PM_APBC_EVSYS;

	HW::PIOA->DIRSET = PA17|PA18|PA19;
	PIO_D_FIRE->DIRSET = D_FIRE;
	PIO_D_FIRE->CLR(D_FIRE);

	PIO_D_FIRE->SetWRCONFIG(D_FIRE,		PORT_PMUX(4) | PORT_WRPINCFG | PORT_WRPMUX);
	PIO_DNNK->SetWRCONFIG(DNNKB|DNNKM,	PORT_PMUX(0) | PORT_WRPINCFG | PORT_PMUXEN | PORT_WRPMUX);

	EIC->EVCTRL |= (EIC_EXTINT0 << DNNKB_EXTINT) | (EIC_EXTINT0 << DNNKM_EXTINT);
	EIC->SetConfig(DNNKB_EXTINT, 0, EIC_SENSE_BOTH);
	EIC->SetConfig(DNNKM_EXTINT, 0, EIC_SENSE_BOTH);
	EIC->INTENCLR = (EIC_EXTINT0 << DNNKB_EXTINT) | (EIC_EXTINT0 << DNNKM_EXTINT);
	EIC->CTRL = EIC_ENABLE;

	EVSYS->USER		= DNNKB_EVSYS_USER;
	EVSYS->CHANNEL	= DNNKB_EVSYS_CHANNEL;

	EVSYS->USER		= DNNKM_EVSYS_USER;
	EVSYS->CHANNEL	= DNNKM_EVSYS_CHANNEL;

	bTC->CTRLA = TC_SWRST;
	mTC->CTRLA = TC_SWRST;
	while((bTC->STATUS|mTC->STATUS) & TC_SYNCBUSY);

	bTC->READREQ = TC_RCONT|0x10;
	mTC->READREQ = TC_RCONT|0x10;

	bTC->EVCTRL = TC_TCEI|TC_EVACT_COUNT;
	mTC->EVCTRL = TC_TCEI|TC_EVACT_COUNT;

	//bTC->CC16[0] = ~1;
	//mTC->CC16[0] = ~1;

	bTC->CTRLA = TC_WAVEGEN_NFRQ|TC_ENABLE;
	mTC->CTRLA = TC_WAVEGEN_NFRQ|TC_ENABLE;

	// Win timer

	//WinTC->CTRLA = TC_SWRST;
	//while(WinTC->STATUS & TC_SYNCBUSY);

	//WinTC->READREQ = TC_RCONT|0x18;

	//WinTC->CC16[0] = windowTime*(MCK_MHz/WIN_DIV) - 1; // временнќе окно
	//WinTC->CC16[1] = 0; // временнќе окно
	//WinTC->EVCTRL = TC_OVFEO;
	//WinTC->CTRLA = WIN_PRESC|TC_WAVEGEN_MFRQ;

	VectorTableExt[WIN_IRQ] = Win_DMA_IRQ;
	CM0::NVIC->ICPR[0] = 1 << WIN_IRQ;
	CM0::NVIC->ISER[0] = 1 << WIN_IRQ;	

	PIOA->SetWRCONFIG(PA18,	PORT_PMUX_E | PORT_WRPINCFG | PORT_PMUXEN | PORT_WRPMUX);

	InitWinLsit();

	//PrepareWin();

	// Gen timer

	GenTCC->CTRLA = TCC_SWRST;

	while(GenTCC->SYNCBUSY);

	VectorTableExt[GEN_IRQ] = GenIRQ;
	CM0::NVIC->ICPR[0] = 1 << GEN_IRQ;
	CM0::NVIC->ISER[0] = 1 << GEN_IRQ;	


	GenTCC->WAVE = TCC_WAVEGEN_NPWM;
	GenTCC->PER = gen_period;
	GenTCC->CC[0] = 2;

	GenTCC->INTENCLR = ~0;
	GenTCC->INTENSET = TCC_OVF;

	GenTCC->INTFLAG = ~0;

	GenTCC->CTRLA = GEN_PRESC;
	GenTCC->CTRLA = GEN_PRESC|TCC_ENABLE;

	SetGenFreq(gen_freq);

	genTimeOut = 60000;

	LoadGenTime();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline void ManDisable()	{ HW::PIOA->CLR(M1|M2);}
inline void ManOne()		{ HW::PIOA->CLR(M1); HW::PIOA->SET(M2); }
inline void ManZero()		{ HW::PIOA->CLR(M2); HW::PIOA->SET(M1); } 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static byte manInv = 0;


static const u16 manbaud[4] = { BAUD2CLK(20833), BAUD2CLK(41666), BAUD2CLK(62500), BAUD2CLK(83333) };//0:20833Hz, 1:41666Hz,2:62500Hz,3:83333Hz


static u16 trmHalfPeriod = (manbaud[0]+1)/2;
static u16 trmHalfPeriod2 = manbaud[0];
static u16 trmHalfPeriod3 = (manbaud[0]*3+1)/2;
static u16 trmHalfPeriod4 = manbaud[0] * 2;
static u16 trmHalfPeriod6 = manbaud[0] * 3;
static u16 trmHalfPeriod7 = (manbaud[0] * 7 + 1) / 2;

byte stateManTrans = 0;
static MTB *manTB = 0;
static bool trmBusy = false;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void SetTrmBoudRate(byte i)
{
	trmHalfPeriod = manbaud[i&3]/2;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static u16 rcvCount = 0;
static bool rcvBusy = false;
byte stateManRcvr = 0;

const u16 rcvPeriod = BAUD2CLK(20833);

static u16* rcvManPtr = 0;
static u16 rcvManCount = 0;


static u16 rcvManLen = 0;
static u32 rcvManPrevTime = 0;

static u16 rcvManLen72 = 0;
static u16 rcvManLen96 = 0;
static u16 rcvManLen24 = 0;
static u16 rcvManLen48 = 0;

static u32 rcvManSum72 = 0;
static u32 rcvManSum96 = 0;
static u32 rcvManSum24 = 0;
static u32 rcvManSum48 = 0;

static u16 rcvManCount72 = 0;
static u16 rcvManCount96 = 0;
static u16 rcvManCount24 = 0;
static u16 rcvManCount48 = 0;

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

			ManTT->CTRLA = 0;
			ManTT->INTENCLR = ~0;

			mltTB->ready = true;
			mltBusy = false;

			break;


	}; // 	switch (stateManTrans)

	ManTT->INTFLAG = ~0;//TCC_OVF;
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

	ManTT->CTRLA = 0;

	VectorTableExt[TCC0_IRQ] = MLT3_TrmIRQ;
	CM0::NVIC->ICPR[0] = 1 << TCC0_IRQ;
	CM0::NVIC->ISER[0] = 1 << TCC0_IRQ;	

	ManTT->PER = trmHalfPeriod-1;
	//ManTT->EVCTRL = 1<<6;

	ManTT->INTENCLR = ~TCC_OVF;
	ManTT->INTENSET = TCC_OVF;

	ManTT->INTFLAG = ~0;

	ManTT->CTRLA = TCC_ENABLE;

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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static __irq void ManTrmIRQ_3()
{
	static u32 tw = 0;
	static u16 count = 0;
	static byte i = 0;
	static const u16* data = 0;
	static u16 len = 0;
	static bool cmd = false;

	Pin_ManTrmIRQ_Set();

	switch (stateManTrans)
	{
		case 0:	// 1-st sync imp 

			data = manTB->data;
			len = manTB->len;
			cmd = false;
			stateManTrans++;

		case 1:

			if (len != 0)
			{
				tw = ((u32)(*data) << 1) | (CheckParity(*data) & 1);

				data++;
				len--;

				count = 17;

				u32 tadd = (cmd) ? trmHalfPeriod : 0;

				ManT_SET_CR(trmHalfPeriod3+tadd);

				if (tw & 0x10000)
				{
					ManT_SET_PR(trmHalfPeriod7+tadd); //US2MT(96);
					stateManTrans += 2;
				}
				else
				{
					ManT_SET_PR(trmHalfPeriod6+tadd); //US2MT(72);
					stateManTrans++;
				};

				//ManTT->CTRLBSET = TCC_CMD_UPDATE;

				tw <<= 1;
				count--;
			}
			else
			{
				stateManTrans = 4;
			};

			break;

		case 2:	

			ManT_SET_CR(trmHalfPeriod);

			if (count == 0)
			{
				ManT_SET_PR(trmHalfPeriod2);
				cmd = false;
				stateManTrans = 1;
			}
			else
			{
				if (tw & 0x10000)
				{
					ManT_SET_PR(trmHalfPeriod3);

					if (count == 1)
					{
						cmd = true;
						stateManTrans = 1;
					}
					else
					{
						stateManTrans++;
					};
				}
				else
				{
					ManT_SET_PR(trmHalfPeriod2);
				};

				tw <<= 1;
				count--;
			};

			//ManTT->CTRLBSET = TCC_CMD_UPDATE;

			break;

		case 3: 

			if (tw & 0x10000)
			{
				ManT_SET_CR(trmHalfPeriod);
				ManT_SET_PR(trmHalfPeriod2);

				tw <<= 1;
				count--;

				if (count == 0)
				{
					cmd = true;
					stateManTrans = 1;
				};
			}
			else
			{
				tw <<= 1;
				count--;

				ManT_SET_CR(trmHalfPeriod2);

				if (tw & 0x10000)
				{
					ManT_SET_PR(trmHalfPeriod4);
					
					if (count == 1)
					{
						cmd = true;
						stateManTrans = 1;
					};
				}
				else
				{
					ManT_SET_PR(trmHalfPeriod3);

					if (count == 0)
					{
						cmd = false;
						stateManTrans = 1;
					}
					else
					{
						stateManTrans--;
					}
				};

//				if (count != 0)
				{
					tw <<= 1;
					count--;
				};
			};

			//ManTT->CTRLBSET = TCC_CMD_UPDATE;

			break;

		case 4:

			stateManTrans++;
			break;

		case 5:

			stateManTrans = 0;

			PIO_MANCH->SetWRCONFIG(M1|M2, PORT_WRPINCFG);

			ManTT->CTRLA &= ~TCC_ENABLE;

			manTB->ready = true;
			trmBusy = false;

			break;


	}; // 	switch (stateManTrans)

	ManTT->INTFLAG = TCC_OVF;

	Pin_ManTrmIRQ_Clr();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool SendManData_3(MTB* mtb)
{
	if (trmBusy || mtb == 0 || mtb->data == 0 || mtb->len == 0)
	{
		return false;
	};

	mtb->ready = false;

	manTB = mtb;

	stateManTrans = 0;

	//trmHalfPeriod = GetTrmBaudRate(mtb->baud);
	trmHalfPeriod2 = trmHalfPeriod * 2 - 1;
	trmHalfPeriod3 = trmHalfPeriod * 3 - 1;
	trmHalfPeriod4 = trmHalfPeriod * 4 - 1;
	trmHalfPeriod6 = trmHalfPeriod * 6 - 1;
	trmHalfPeriod7 = trmHalfPeriod * 7 - 1;

	stateManTrans = 0;

	ManTT->CTRLA = MAN_PRESC;
	ManTT->WAVE = TCC_WAVEGEN_NPWM;//|TCC_POL0;
	ManTT->DRVCTRL = TCC_INVEN1;

	ManTT->PER = US2MT(50)-1;
	ManTT->CC[0] = 0; 
	ManTT->CC[1] = ~0; 

	ManTT->EVCTRL = 0;

	ManTT->INTENCLR = ~0;
	ManTT->INTENSET = TCC_OVF;
	ManTT->INTFLAG = TCC_OVF;

	ManTT->CTRLA = MAN_PRESC|TCC_ENABLE;
	ManTT->CTRLBSET = TCC_CMD_RETRIGGER;

	PIO_MANCH->SetWRCONFIG(M1|M2, PORT_PMUX_E|PORT_WRPMUX|PORT_WRPINCFG|PORT_PMUXEN);
	PIO_MANCH->CLR(M1|M2);
	PIO_MANCH->DIRSET = M1|M2;

	return trmBusy = true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void InitManTransmit_3()
{
	using namespace HW;

	VectorTableExt[MANT_IRQ_2] = ManTrmIRQ_3;
	CM0::NVIC->CLR_PR(MANT_IRQ_2);
	CM0::NVIC->SET_ER(MANT_IRQ_2);

	HW::GCLK->CLKCTRL = GCLK_ID_TCC0_TCC1 | GCLK_GEN(GEN_MCK) | GCLK_CLKEN;

	while (HW::GCLK->STATUS);

	PM->APBCMASK |= PM_APBC_TCC0;

	PIO_MANCH->CLR(M1|M2);
	PIO_MANCH->DIRSET = M1|M2;
	PIO_MANCH->SetWRCONFIG(M1|M2, PORT_WRPINCFG);

	PIOA->SetWRCONFIG(PA04|PA05, PORT_PMUX_E|PORT_WRPMUX|PORT_WRPINCFG|PORT_PMUXEN);

	PIOA->DIRSET = PA10|PA11;

	ManTT->CTRLA = TCC_SWRST;

	while(ManTT->SYNCBUSY);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void ManRcvEnd(bool ok)
{
	ManRT->INTENCLR = ~0;

	manRB->OK = ok;
	manRB->ready = true;
	manRB->len = rcvManLen;

	rcvManLen96 = (rcvManCount96 != 0) ? (rcvManSum96 / rcvManCount96) : 0;
	rcvManLen72 = (rcvManCount72 != 0) ? (rcvManSum72 / rcvManCount72) : 0;
	rcvManLen48 = (rcvManCount48 != 0) ? (rcvManSum48 / rcvManCount48) : 0;
	rcvManLen24 = (rcvManCount24 != 0) ? (rcvManSum24 / rcvManCount24) : 0;
	
	rcvBusy = false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//static Receiver manRcv(true);

u32 lastCaptureValue = 0;
byte manRcvState = 0;

u32 manRcvTime1 = 0;
u32 manRcvTime2 = 0;
u32 manRcvTime3 = 0;
u32 manRcvTime4 = 0;

static RTM32 manRcvTime;

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

	Pin_ManRcvIRQ_Set();

	u32 len = ManRT->CC[0];

	_state = !_state;

	if (len <= MT(60))
	{
		if (len <= MT(36))
		{
			_length += 1; rcvManSum24 += len; rcvManCount24++;
		}
		else
		{
			_length += 2; rcvManSum48 += len; rcvManCount48++;
		};

		if(_length >= 3) { _sync = false; };
	}
	else
	{
		if(len > MT(108))
		{
			_sync = false;
		}
		else
		{
			manRcvTime.Reset();

			_sync = true;
			_data = 0;
			_parity_temp = _parity;
			_number = 0;
			_command = !_state; 

			if (len <= MT(84))
			{
				_length = 1; rcvManSum72 += len; rcvManCount72++;
			}
			else
			{
				_length = 2; rcvManSum96 += len; rcvManCount96++;
			};

		};
	};

	if(_sync && _length == 2)
	{
		manRcvTime.Reset();

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
			Pin_ManRcvSync_Set();

			_sync = false;

			if(_state != _parity_temp)
			{
				_state = !_state;
				_data = (~_data);
				_command = !_command;
			};

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

			Pin_ManRcvSync_Clr();
		};
	};

	ManRT->INTFLAG = ~0;

	Pin_ManRcvIRQ_Clr();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ManRcvUpdate()
{
	if (rcvBusy)
	{
		__disable_irq();

		if (rcvManLen > 0 && manRcvTime.Timeout(US2RT(200)))
		{
			ManRcvEnd(true);
		}
		else
		{
			manRB->len = rcvManLen;
		};

		__enable_irq();
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void InitManRecieve()
{
	using namespace HW;

	HW::GCLK->CLKCTRL = GCLK_ID_TCC0_TCC1					| GCLK_GEN(GEN_MCK)	| GCLK_CLKEN;
	HW::GCLK->CLKCTRL = GCLK_ID_TC6_TC7						| GCLK_GEN(GEN_MCK)	| GCLK_CLKEN;
	HW::GCLK->CLKCTRL = (GCLK_ID_EVSYS_0 + EVENT_MANR_1)	| GCLK_GEN(GEN_MCK)	| GCLK_CLKEN; // NNKB
	HW::GCLK->CLKCTRL = (GCLK_ID_EVSYS_0 + EVENT_MANR_2)	| GCLK_GEN(GEN_MCK)	| GCLK_CLKEN; // NNKM

	while (HW::GCLK->STATUS);

	PM->APBCMASK |= PM_APBC_TCC1 | PM_APBC_TC6;
	PM->APBCMASK |= PM_APBC_EVSYS;

	PIO_MANCH->DIRCLR = TLS;
	PIO_MANCH->CTRL |= TLS;

	PIO_MANCH->SetWRCONFIG(TLS, PORT_PMUX_A|PORT_WRPINCFG|PORT_PMUXEN|PORT_WRPMUX|PORT_INEN);
	PIOB->SetWRCONFIG(PB02|PB03, PORT_PMUX_E|PORT_WRPINCFG|PORT_PMUXEN|PORT_WRPMUX);

	EIC->EVCTRL |= EIC_EXTINT0 << MANR_EXTINT;
	EIC->SetConfig(MANR_EXTINT, 0, EIC_SENSE_LOW);
	EIC->INTENCLR = EIC_EXTINT0 << MANR_EXTINT;
	EIC->CTRL = EIC_ENABLE;

	EVSYS->CHANNEL	= MANIT_EVSYS_CHANNEL;
	EVSYS->USER		= MANIT_EVSYS_USER;		//__dsb(15);

	EVSYS->CHANNEL	= MANR_EVSYS_CHANNEL;
	EVSYS->USER		= MANR_EVSYS_USER1;		//__dsb(15);
	EVSYS->USER		= MANR_EVSYS_USER2;		//__dsb(15);

	ManRT->CTRLA = TCC_SWRST;
	while(ManRT->SYNCBUSY);

	VectorTableExt[MANR_IRQ] = ManRcvIRQ2;
	CM0::NVIC->CLR_PR(MANR_IRQ);
	CM0::NVIC->SET_ER(MANR_IRQ);	

	ManRT->CTRLA = TCC_CPTEN0|MAN_PRESC;
	ManRT->EVCTRL = TCC_MCEI0|TCC_TCEI0|TCC_EVACT0_RETRIGGER;

	ManRT->PER = ~0;
	ManRT->CC[1] = 250;

	ManRT->INTENCLR = ~0;

	ManRT->CTRLA = TCC_CPTEN0|MAN_PRESC|TCC_ENABLE;

	ManIT->CTRLA = TCC_SWRST;
	while(ManIT->STATUS & TC_SYNCBUSY);

	ManIT->CTRLA = TC_WAVEGEN_NPWM|TC_MODE_COUNT8|MAN_PRESC;

	ManIT->EVCTRL = TC_TCEI|TC_EVACT_RETRIGGER|TC_OVFEO;

	ManIT->PER8 = MT(12)-1;
	ManIT->CC8[0] = 1;
	ManIT->CC8[1] = MT(12)-2;

	ManIT->INTENCLR = ~0;

	ManIT->INTFLAG = ~0;

	ManIT->CTRLA |= TC_ENABLE;

	ManIT->CTRLBSET = TC_ONESHOT;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool RcvManData(MRB *mrb)
{
	if (rcvBusy /*|| trmBusy*/ || mrb == 0 || mrb->data == 0 || mrb->maxLen == 0)
	{
		return false;
	};

	mrb->ready = mrb->OK = false;
	mrb->len = 0;

	manRB = mrb;
	
	rcvManLen = 0;

	rcvManPtr = manRB->data;
	rcvManCount = manRB->maxLen;

	rcvManSum96 = 0; rcvManCount96 = 0;
	rcvManSum72 = 0; rcvManCount72 = 0;
	rcvManSum48 = 0; rcvManCount48 = 0;
	rcvManSum24 = 0; rcvManCount24 = 0;

	ManRT->INTFLAG = ~0;
	ManRT->INTENSET = TCC_MC0;

	return rcvBusy = true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

List<DSCI2C>	i2c_ReqList;
DSCI2C*			i2c_dsc = 0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void Upadte_I2C()
{
	enum STATE { WAIT = 0, WRITE, READ, STOP };

	static STATE state = WAIT;
	static byte *wrPtr = 0;
	static byte *rdPtr = 0;
	static u16 	wrCount = 0;
	static u16 	rdCount = 0;
	static byte *wrPtr2 = 0;
	static u16	wrCount2 = 0;
	static byte adr = 0;

	switch (state)
	{
		case WAIT:

			i2c_dsc = i2c_ReqList.Get();

			if (i2c_dsc != 0)
			{
				DSCI2C &dsc = *i2c_dsc;

				dsc.ready = false;
				dsc.ack = false;

				wrPtr = (byte*)dsc.wdata;	
				rdPtr = (byte*)dsc.rdata;	
				wrPtr2 = (byte*)dsc.wdata2;	
				wrCount = dsc.wlen;
				wrCount2 = dsc.wlen2;
				rdCount = dsc.rlen;
				adr = dsc.adr;

				if (wrPtr2 == 0) wrCount2 = 0;

				I2C->CTRLB = I2C_SMEN;
				I2C->STATUS.BUSSTATE = BUSSTATE_IDLE;

				I2C->INTFLAG = ~0;

				if (wrCount == 0)
				{
					I2C->ADDR = (adr << 1) | 1;
					state = READ; 
				}
				else
				{
					I2C->ADDR = (adr << 1);
					state = WRITE; 
				};
			};

			break;

		case WRITE:

			if(I2C->INTFLAG & I2C_MB) // Data can be transmitted 
			{
				DSCI2C &dsc = *i2c_dsc;

				dsc.ack = true;

				if (wrCount > 0)
				{
					I2C->DATA = *wrPtr++;

					wrCount--;

					if(wrCount == 0 && wrCount2 != 0)
					{
						wrPtr = wrPtr2;
						wrCount = wrCount2;
						wrCount2 = 0;
					};
				}
				else if (rdCount > 0)
				{
					I2C->ADDR = (adr << 1) | 1;
	
					state = READ; 
				}
				else
				{
					I2C->CTRLB = I2C_SMEN|I2C_ACKACT|I2C_CMD_STOP;
					
					state = STOP; 
				};
			}
			else if(I2C->INTFLAG & I2C_ERROR)
			{
				I2C->CTRLB = I2C_SMEN|I2C_CMD_STOP;
				
				state = STOP; 
			};

			break;

		case READ:

			if(I2C->INTFLAG & I2C_SB) // Received data is available
			{
				DSCI2C &dsc = *i2c_dsc;

				dsc.ack = true;

				*rdPtr++ = I2C->DATA; // receive data

				rdCount--;

				//if (rdCount == 1) I2C->CTRLB = I2C_SMEN|I2C_ACKACT;

				if (rdCount > 0)
				{
					I2C->CTRLB = I2C_SMEN|I2C_CMD_2; 
				}
				else
				{
					I2C->CTRLB = I2C_SMEN|I2C_ACKACT|I2C_CMD_STOP; 
					state = STOP; 
				};
			}
			else if(I2C->INTFLAG & I2C_ERROR)
			{
				I2C->CTRLB = I2C_SMEN|I2C_ACKACT|I2C_CMD_STOP;
				
				state = STOP; 
			};

			break;

		case STOP:

			if (I2C->STATUS.BUSSTATE == BUSSTATE_IDLE)
			{
				i2c_dsc->ready = true;
				
				i2c_dsc = 0;
				
				I2C->CTRLB = I2C_SMEN;

				state = WAIT; 
			}
			else if (I2C->SYNCBUSY == 0)
			{
				I2C->CTRLB = I2C_SMEN|I2C_ACKACT|I2C_CMD_STOP;
			};

			break;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Upadte_I2C_DMA()
{
	enum STATE { WAIT = 0, WRITE, READ, STOP };

	static STATE state = WAIT;
	static byte *wrPtr = 0;
	static byte *rdPtr = 0;
	static u16 	wrCount = 0;
	static u16 	rdCount = 0;
	static byte *wrPtr2 = 0;
	static u16	wrCount2 = 0;
	static byte adr = 0;
	static __align(16) T_HW::DMADESC wr_dmadsc;

	switch (state)
	{
		case WAIT:

			i2c_dsc = i2c_ReqList.Get();

			if (i2c_dsc != 0)
			{
				DSCI2C &dsc = *i2c_dsc;

				dsc.ready = false;
				dsc.ack = false;

				wrPtr = (byte*)dsc.wdata;	
				rdPtr = (byte*)dsc.rdata;	
				wrPtr2 = (byte*)dsc.wdata2;	
				wrCount = dsc.wlen;
				wrCount2 = dsc.wlen2;
				rdCount = dsc.rlen;
				adr = dsc.adr;

				if (wrPtr2 == 0) wrCount2 = 0;

				I2C->CTRLB = I2C_SMEN;
				I2C->STATUS.BUSSTATE = BUSSTATE_IDLE;

				I2C->INTFLAG = ~0;

				T_HW::DMADESC &dmadsc = DmaTable[I2C_DMACH];

				if (wrCount == 0)
				{
					dmadsc.SRCADDR	= &I2C->DATA;
					dmadsc.DSTADDR	= rdPtr + rdCount;
					dmadsc.DESCADDR = 0;
					dmadsc.BTCNT	= rdCount;
					dmadsc.BTCTRL	= DMDSC_VALID|DMDSC_BEATSIZE_BYTE|DMDSC_DSTINC;

					__disable_irq();

					HW::DMAC->CHID = I2C_DMACH;
					HW::DMAC->CHCTRLB = DMCH_TRIGACT_BEAT|I2C_TRIGSRC_RX;
					HW::DMAC->CHINTENCLR = ~0;
					HW::DMAC->CHINTFLAG = ~0;
					HW::DMAC->CHCTRLA = DMCH_ENABLE;

					__enable_irq();

					I2C->ADDR = ((rdCount <= 255) ? (I2C_LEN(rdCount)|I2C_LENEN) : 0) | (adr << 1) | 1;
					state = READ; 
				}
				else
				{
					dmadsc.SRCADDR	= wrPtr + wrCount;
					dmadsc.DSTADDR	= &I2C->DATA;
					dmadsc.BTCNT	= wrCount;
					dmadsc.BTCTRL	= DMDSC_VALID|DMDSC_BEATSIZE_BYTE|DMDSC_SRCINC;

					if (wrCount2 == 0)
					{
						dmadsc.DESCADDR = 0;
					}
					else
					{
						wr_dmadsc.SRCADDR	= wrPtr2 + wrCount2;
						wr_dmadsc.DSTADDR	= &I2C->DATA;
						wr_dmadsc.BTCNT		= wrCount2;
						wr_dmadsc.BTCTRL	= DMDSC_VALID|DMDSC_BEATSIZE_BYTE|DMDSC_SRCINC;
						dmadsc.DESCADDR		= &wr_dmadsc;
					};

					__disable_irq();

					HW::DMAC->CHID = I2C_DMACH;
					HW::DMAC->CHCTRLB = DMCH_TRIGACT_BEAT|I2C_TRIGSRC_TX;
					HW::DMAC->CHINTENCLR = ~0;
					HW::DMAC->CHINTFLAG = ~0;
					HW::DMAC->CHCTRLA = DMCH_ENABLE;

					__enable_irq();

					I2C->ADDR = (adr << 1);
					state = WRITE; 
				};
			};

			break;

		case WRITE:

			if((I2C->INTFLAG & I2C_ERROR) || I2C->STATUS.RXNACK)
			{
				I2C->CTRLB = I2C_SMEN|I2C_CMD_STOP;
				
				state = STOP; 
			}
			else
			{
				DSCI2C &dsc = *i2c_dsc;

				__disable_irq();

				HW::DMAC->CHID = I2C_DMACH;

				bool c = ((HW::DMAC->CHCTRLA & DMCH_ENABLE) == 0 || (HW::DMAC->CHINTFLAG & DMCH_TCMPL)) && (I2C->INTFLAG & I2C_MB);
				
				__enable_irq();

				if (c)
				{
					dsc.ack = true;

					if (rdCount > 0)
					{
						T_HW::DMADESC &dmadsc = DmaTable[I2C_DMACH];

						dmadsc.SRCADDR	= &I2C->DATA;
						dmadsc.DSTADDR	= rdPtr + rdCount;
						dmadsc.DESCADDR = 0;
						dmadsc.BTCNT	= rdCount;
						dmadsc.BTCTRL	= DMDSC_VALID|DMDSC_BEATSIZE_BYTE|DMDSC_DSTINC;

						__disable_irq();

						HW::DMAC->CHID = I2C_DMACH;
						HW::DMAC->CHCTRLB = DMCH_TRIGACT_BEAT|I2C_TRIGSRC_RX;
						HW::DMAC->CHINTENCLR = ~0;
						HW::DMAC->CHINTFLAG = ~0;
						HW::DMAC->CHCTRLA = DMCH_ENABLE;

						__enable_irq();

						I2C->ADDR = ((rdCount <= 255) ? (I2C_LEN(rdCount)|I2C_LENEN) : 0) | (adr << 1) | 1;
		
						state = READ; 
					}
					else
					{
						I2C->CTRLB = I2C_SMEN|I2C_ACKACT|I2C_CMD_STOP;
						
						state = STOP; 
					};
				};
			};

			break;

		case READ:

			if((I2C->INTFLAG & I2C_ERROR) || I2C->STATUS.RXNACK)
			{
				I2C->CTRLB = I2C_SMEN|I2C_ACKACT|I2C_CMD_STOP;
				
				state = STOP; 
			}
			else
			{
				DSCI2C &dsc = *i2c_dsc;

				__disable_irq();

				HW::DMAC->CHID = I2C_DMACH;

				bool c = (HW::DMAC->CHCTRLA & DMCH_ENABLE) == 0 || (HW::DMAC->CHINTFLAG & DMCH_TCMPL);
				
				__enable_irq();

				if (c)
				{
					dsc.ack = true;

					I2C->CTRLB = I2C_SMEN|I2C_ACKACT|I2C_CMD_STOP;
						
					state = STOP; 
				};
			};

			break;

		case STOP:

			if (I2C->STATUS.BUSSTATE == BUSSTATE_IDLE)
			{
				i2c_dsc->ready = true;
				
				i2c_dsc = 0;
				
				I2C->CTRLB = I2C_SMEN;

				state = WAIT; 
			}
			else if (I2C->SYNCBUSY == 0)
			{
				I2C->CTRLB = I2C_SMEN|I2C_ACKACT|I2C_CMD_STOP;
			};

			break;
	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool I2C_AddRequest(DSCI2C *d)
{
	using namespace HW;

	if (d == 0) { return false; };
	if ((d->wdata == 0 || d->wlen == 0) && (d->rdata == 0 || d->rlen == 0)) { return false; }


	d->ready = false;

	i2c_ReqList.Add(d);

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool Init_I2C()
{
	using namespace HW;

	HW::GCLK->CLKCTRL = GCLK_ID_SERCOM3_CORE|GCLK_GEN(GEN_MCK)|GCLK_CLKEN;

	PM->APBCMASK |= PM_APBC_SERCOM3;

	PIO_I2C->SetWRCONFIG(SCL|SDA, PORT_PMUX_C | PORT_PMUXEN | PORT_WRPMUX | PORT_PULLEN | PORT_WRPINCFG);
	PIO_I2C->SET(SCL|SDA);

	I2C->CTRLA = I2C_SWRST;

	while(I2C->SYNCBUSY);

	I2C->CTRLA = SERCOM_MODE_I2C_MASTER;

	I2C->CTRLA = SERCOM_MODE_I2C_MASTER|I2C_INACTOUT_205US|I2C_SPEED_SM;
	I2C->CTRLB = I2C_SMEN;
	I2C->BAUD = 0x3030;

	I2C->CTRLA |= I2C_ENABLE;

	while(I2C->SYNCBUSY);

	I2C->STATUS = 0;
	I2C->STATUS.BUSSTATE = BUSSTATE_IDLE;

	//VectorTableExt[SERCOM3_IRQ] = Handler_I2C;
	//CM0::NVIC->ICPR[0] = 1 << SERCOM3_IRQ;
	//CM0::NVIC->ISER[0] = 1 << SERCOM3_IRQ;

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 ad5312_data[2] = { 2048, 2048 };

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void AD5312_Set(byte channel, u16 dac)
{
	ad5312_data[channel&1] = dac & 0x3FF;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void Init_AD5312()
{
	using namespace HW;
	
	HW::GCLK->CLKCTRL = GCLK_ID_SERCOM5_CORE|GCLK_GEN(GEN_MCK)|GCLK_CLKEN;
	HW::GCLK->CLKCTRL = GCLK_ID_SERCOMX_SLOW|GCLK_GEN(GEN_32K)|GCLK_CLKEN;

	PM->APBCMASK |= PM_APBC_SERCOM5;

	PIO_SPI->SetWRCONFIG(SCLK|MOSI, PORT_PMUX_C|PORT_WRPINCFG|PORT_PMUXEN|PORT_WRPMUX);

	PIO_SYNC->DIRSET = SYNC;

	SPI->CTRLA = SERCOM_MODE_SPI_MASTER|SPI_FORM_SPI|SPI_DIPO(0)|SPI_DOPO(0)|SPI_CPHA|SPI_DORD_MSB;

	SPI->CTRLB = SPI_CHSIZE_8BIT;
	SPI->BAUD = 23;

	SPI->CTRLA |= SPI_ENABLE;

	while(SPI->SYNCBUSY);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void Update_AD5312()
{
	static byte state = 0;
	static byte chn = 0;
	static U16u data(0);

	static TM32 tm;

	switch (state)
	{
		case 0:

			if (tm.Check(101))
			{
				data.w = (chn<<15) | (ad5312_data[chn] << 2); chn = (chn + 1) & 1;

				PIO_SYNC->CLR(SYNC);

				SPI->DATA = data.b[1];

				while ((SPI->INTFLAG & SPI_DRE) == 0);

				SPI->DATA = data.b[0];

				state++;
			};

			break;

		case 1:

			if (SPI->INTFLAG & SPI_DRE)
			{
				//SPI->DATA = data.b[0];

				state++;
			};

			break;

		case 2:

			if (SPI->INTFLAG & SPI_TXC)
			{
				PIO_SYNC->SET(SYNC);

				state = 0;
			};

			break;

	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void InitHardware()
{
	using namespace HW;

	HW::PIOA->BSET(13);

//	InitVectorTable();

	HW::GCLK->CLKCTRL = GCLK_ID_EIC				| GCLK_GEN(GEN_MCK) | GCLK_CLKEN;
	HW::GCLK->CLKCTRL = GCLK_ID_SERCOMX_SLOW	| GCLK_GEN(GEN_32K) | GCLK_CLKEN;

	PM->APBAMASK |= PM_APBA_EIC;
	PM->APBCMASK |= PM_APBC_EVSYS;
	
	EIC->CTRL = EIC_SWRST;

	while(EIC->STATUS);

	Init_time();
	Init_I2C();
	InitManTransmit_3();
	InitManRecieve();
	InitGen();
	Init_AD5312();

#ifndef _DEBUG

	if ((WDT->CTRL & (WDT_ALWAYSON|WDT_ENABLE)) == 0)
	{
		WDT->CONFIG = WDT_PER_16K;				// 16384 clock cycles
		WDT->CTRL = WDT_ALWAYSON|WDT_ENABLE;	// Always on, enabled
	};

	ResetWDT();

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
		CALL( Upadte_I2C_DMA();		);
	};

	i = (i > (__LINE__-S-3)) ? 0 : i;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
