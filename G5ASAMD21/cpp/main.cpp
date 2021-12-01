#include "hardware.h"
#include "time.h"
#include "options.h"

#define VERSION			0x0102

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

const char build_date[] __attribute__((used)) = "\n" __DATE__ "\n" __TIME__ "\n";

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static u16 temp = 0;

inline u16 ReverseWord(u16 v) { __asm	{ rev16 v, v };	return v; }

static void* eepromData = 0;
static u16 eepromWriteLen = 0;
static u16 eepromReadLen = 0;
static u16 eepromStartAdr = 0;

static MTB mtb;

static u16 manBuf[16];

u16 manRcvData[10];
u16 manTrmData[50];

u16 txbuf[128 + 512 + 16];


static u16 manReqWord = 0x7000;
static u16 manReqMask = 0xFF00;

static u16 numDevice = 0;
static u16 verDevice = VERSION;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan_00(const u16 *data, u16 len, MTB &mtb)
{
	if (len != 1) return false;

	txbuf[0] = (manReqWord & manReqMask) | 0;
	txbuf[1] = Options_Get_Serial();
	txbuf[2] = VERSION;

	mtb.data = txbuf;
	mtb.len = 3;
	mtb.next = 0;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan_10(const u16 *data, u16 len, MTB &mtb)
{
	if (len != 1) return false;

	txbuf[0] = (manReqWord & manReqMask) | 0x10;

	txbuf[1] = GetGenFreq();					//	Частота генератора(Гц), 
	txbuf[2] = GetWindowCount();				//	Количество временных окон(шт), 
	txbuf[3] = GetWindowTime();					//	Длительность временного окна(мкс), 
	txbuf[4] = Options_Get_Level_M();			//	Уровень дискриминации МЗ(у.е), 
	txbuf[5] = Options_Get_Level_B();			//	Уровень дискриминации БЗ(у.е),

	mtb.data = txbuf;
	mtb.len = 6;
	mtb.next = 0;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


static bool RequestMan_20(const u16 *data, u16 len, MTB &mtb)
{
	if (len != 1) return false;

	if (data[0] & 1)
	{
		EnableGen();
	}
	else
	{
		DisableGen();
	};

	txbuf[0] = data[0];//(manReqWord & manReqMask) | 0x20;

	txbuf[1] = GetFireCount();								//2. Количество вспышек(ushort)
	txbuf[2] = GetGenWorkTime();							//3. Наработка генератора(мин)(ushort)
	txbuf[3] = temp;										//4. Температура в приборе(0.1 гр)(short)
	byte wc = txbuf[4] = GetWindowCount();					//5. Количество временных окон(шт)
	txbuf[5] = GetWindowTime();								//6. Длительность временного окна(мкс)

	u16 n = 6;

	for (byte i = 0; i < wc; i++)
	{
		__disable_irq();
		u32 tm = m_ts[i];
		u32 tb = b_ts[i];
		m_ts[i] = 0;
		b_ts[i] = 0;
		__enable_irq();

		txbuf[n] = (tm < 0xFFFF) ? tm : 0xFFFF;				//7..x. Спектр МЗ(ushort)
		txbuf[n+wc] = (tb < 0xFFFF) ? tb : 0xFFFF;			//x..y. Спектр БЗ(ushort)
		n += 1;
	};

	mtb.data = txbuf;
	mtb.len = 6 + GetWindowCount() * 2;
	mtb.next = 0;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan_80(const u16 *data, u16 len, MTB &mtb)
{
//	PORTB.OUTSET = PIN2_bm; PORTB.OUTCLR = PIN2_bm; 


	if (len != 3) return false;

//	PORTB.OUTSET = PIN2_bm; PORTB.OUTCLR = PIN2_bm; 

	switch (data[1])
	{
		case 1:

			//Options_Set_Serial(data[2]);

			break;

		case 2:

			//for (u16 j = 0; j < (data[2]+1); j++) { PORTB.OUTSET = PIN2_bm; PORTB.OUTCLR = PIN2_bm; };

			Options_Set_Telemetry_Baud_Rate(data[2]);

			SetTrmBoudRate(Options_Get_Telemetry_Baud_Rate()-1);

			break;

		default:

			return false;
	};

	txbuf[0] = (manReqWord & manReqMask) | 0x80;

	mtb.data = txbuf;
	mtb.len = 1;
	mtb.next = 0;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan_90(const u16 *data, u16 len, MTB &mtb)
{
	if (len != 3) return false;

	switch (data[1])
	{
		case 1:

			SetGenFreq(data[2]);

			break;

		case 2:

			SetWindowCount(data[2]);

			break;

		case 3:

			SetWindowTime(data[2]);

			break;

		case 4:

			Options_Set_Level_M(data[2]);

			break;

		case 5:

			Options_Set_Level_B(data[2]);

			break;

		default:

			return false;
	};

	txbuf[0] = (manReqWord & manReqMask) | 0x90;

	mtb.data = txbuf;
	mtb.len = 1;
	mtb.next = 0;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan_A0(const u16 *data, u16 len, MTB &mtb)
{
	if (len != 3) return false;

	switch (data[1])
	{
		case 1:

			ResetGenWorkTime();

			break;

		default:

			return false;
	};

	txbuf[0] = (manReqWord & manReqMask) | 0xA0;

	mtb.data = txbuf;
	mtb.len = 1;
	mtb.next = 0;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan_F0(const u16 *data, u16 len, MTB &mtb)
{
	if (len != 1) return false;

	txbuf[0] = (manReqWord & manReqMask) | 0xF0;

	mtb.data = txbuf;
	mtb.len = 1;
	mtb.next = 0;

	Options_Save();

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan(const u16 *buf, u16 len, MTB &mtb)
{

	if (buf == 0 || len == 0) return false;

//	PORTB.OUTSET = PIN2_bm;

	bool r = false;

	byte i = ((byte)buf[0] >> 4) & 0xF;

//	for (byte j = 0; j < (i+1); j++) { PORTB.OUTSET = PIN2_bm; PORTB.OUTCLR = PIN2_bm; };

	switch (i)
	{
		case 0: 	r = RequestMan_00(buf, len, mtb); break;
		case 1: 	r = RequestMan_10(buf, len, mtb); break;
		case 2: 	r = RequestMan_20(buf, len, mtb); break;

		case 8: 	r = RequestMan_80(buf, len, mtb); break;
		case 9:		r = RequestMan_90(buf, len, mtb); break;
		case 0xA:	r = RequestMan_A0(buf, len, mtb); break;
		case 0xF:	r = RequestMan_F0(buf, len, mtb); break;
		
//		default:	bfURC++; 
	};

	return r;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void UpdateTemp()
{
	static byte i = 0;

	static DSCI2C dsc;
	static byte reg = 0;
	static u16 rbuf = 0;
	static byte *romData = 0;
	static u16 romAdr = 0;
	static u16 revRomAdr = 0;
	static u16 romWrLen = 0;
	static u16 romRdLen = 0;
	static u16 pageLen = 0;

	static TM32 tm;

//	HW::GPIO->SET0 = 1<<12;

	switch (i)
	{
		case 0:

			if (eepromWriteLen != 0)
			{
				if (eepromData == 0)
				{
					eepromWriteLen = 0;
				}
				else
				{
					romData = (byte*)eepromData;
					romWrLen = eepromWriteLen;
					romAdr = eepromStartAdr;
					revRomAdr = ReverseWord(romAdr);

					i = 2;
				};
			}
			else if (eepromReadLen != 0)
			{
				if (eepromData == 0)
				{
					eepromReadLen = 0;
				}
				else
				{
					romData = (byte*)eepromData;
					romRdLen = eepromReadLen;
					romAdr = eepromStartAdr;
					revRomAdr = ReverseWord(romAdr);

					i = 5;
				};
			}
			else if (tm.Check(200))
			{
				dsc.wdata = &reg;
				dsc.wlen = 1;
				dsc.rdata = &rbuf;
				dsc.rlen = 2;
				dsc.adr = 0x49;
				dsc.wdata2 = 0;
				dsc.wlen2 = 0;


				if (Write_I2C(&dsc))
				{
					i++;
				};
			};

			break;

		case 1:

			if (Check_I2C_ready())
			{
				i32 t = (i16)ReverseWord(rbuf);
				temp = (t * 10 + 64) / 128;

				i = 0;
			};

			break;

		case 2:		// Write at24c128

			pageLen = (romWrLen > 64) ? 64 : romWrLen;

			dsc.wdata = &revRomAdr;
			dsc.wlen = sizeof(revRomAdr);
			dsc.wdata2 = romData;
			dsc.wlen2 = pageLen;
			dsc.rdata = 0;
			dsc.rlen = 0;
			dsc.adr = 0x50;

			if (Write_I2C(&dsc))
			{
				tm.Reset();

				i++;
			};

			break;

		case 3:

			if (Check_I2C_ready())
			{
				tm.Reset();

				i++;
			};

			break;

		case 4:

			if (tm.Check(10))
			{
				romWrLen -= pageLen;
				romData += pageLen;
				revRomAdr = ReverseWord(romAdr += pageLen);

				if (romWrLen > 0)
				{
					i = 2;
				}
				else
				{
					eepromWriteLen = 0;

					i = 0;
				};

			};

			break;

		case 5:		// Read at24c128

			dsc.wdata = &revRomAdr;
			dsc.wlen = sizeof(revRomAdr);
			dsc.wdata2 = 0;
			dsc.wlen2 = 0;
			dsc.rdata = romData;
			dsc.rlen = romRdLen;
			dsc.adr = 0x50;

			if (Read_I2C(&dsc))
			{
				tm.Reset();

				i++;
			};

			break;

		case 6:

			if (Check_I2C_ready())
			{
				eepromReadLen = 0;

				i = 0;
			};

			break;
	};

//	HW::GPIO->CLR0 = 1<<12;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void UpdateMan()
{
	static MTB mtb;
	static MRB mrb;

	static byte i = 0;

	static RTM32 tm;
	static TM32 tm2;


//	u16 c;

	//if (tm2.Check(20))
	//{ 
	//	manBuf[0] = 0xAA00;
	//	manBuf[1] = 0xA5A5;
	//	manBuf[2] = 0x5555;
	//	manBuf[3] = 0x1111;
	//	manBuf[4] = 0x2222;
	//	manBuf[5] = 0x3333;

	//	mtb.data1 = manBuf;
	//	mtb.len1 = 6;

	//	mtb.data2 = 0;
	//	mtb.len2 = 0;

	//	SendManData(&mtb);
	//};

	switch (i)
	{
		case 0:

			mrb.data = manRcvData;
			mrb.maxLen = 10;
			RcvManData(&mrb);

			i++;

			break;

		case 1:

			ManRcvUpdate();

			if (mrb.ready)
			{
				tm.Reset();

				if (mrb.OK && mrb.len > 0 && (manRcvData[0] & manReqMask) == manReqWord && RequestMan(manRcvData, mrb.len, mtb))
				{
					i++;
				}
				else
				{
//					HW::PIOB->SODR = 1<<10;
					i = 0;
//					HW::PIOB->CODR = 1<<10;
				};
			}
			else if (mrb.len > 0)
			{

			};

			break;

		case 2:

			if (tm.Check(US2RT(100)))
			{
//				SetTrmBoudRate(3); /*mtb.data = tableCRC;*/ mtb.len = 5; SendMLT3(&mtb);
				SendManData_2(&mtb);

				i++;
			};

			break;

		case 3:

			if (mtb.ready)
			{
				i = 0;
			};

			break;

	};
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void UpdateParams()
{
	static byte i = 0;

	#define CALL(p) case (__LINE__-S): p; break;

	enum C { S = (__LINE__+3) };
	switch(i++)
	{
		CALL( UpdateTemp()			);
		CALL( UpdateMan()			);
		CALL( UpdateHardware()		);
	};

	i = (i > (__LINE__-S-3)) ? 0 : i;

	#undef CALL
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void CopyDataDMA(volatile void *src, volatile void *dst, u16 len)
{
	using namespace HW;

	register u32 t __asm("r0");
		
	DMAC->CHID = 0;
	DMAC->CHCTRLA = 0;
	DMAC->CHCTRLA = DMCH_SWRST;

	DmaTable[0].SRCADDR = (byte*)src+len;
	DmaTable[0].DSTADDR = (byte*)dst+len;
	DmaTable[0].DESCADDR = 0;//&DmaTable[1];
	DmaTable[0].BTCNT = len;
	DmaTable[0].BTCTRL = DMDSC_VALID|DMDSC_BEATSIZE_BYTE|DMDSC_DSTINC/*|DMDSC_SRCINC*/;

	//DmaTable[1].SRCADDR = src;
	//DmaTable[1].DSTADDR = dst;
	//DmaTable[1].DESCADDR = 0;
	//DmaTable[1].BTCNT = len;
	//DmaTable[1].BTCTRL = DMDSC_VALID|DMDSC_BEATSIZE_BYTE|DMDSC_DSTINC|DMDSC_SRCINC|DMDSC_BLOCKACT_INT;


	DMAC->CHCTRLB = DMCH_TRIGACT_TRANSACTION|DMCH_TRIGSRC_DISABLE;
	DMAC->CHCTRLA = DMCH_ENABLE;
	DMAC->SWTRIGCTRL = 1;

	while(DMAC->CHCTRLA & DMCH_ENABLE);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//inline bool CheckDataComplete()
//{
//	return (HW::DMAC->CH[0].CHCTRLA & DMCH_ENABLE) == 0;
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int main()
{
	static bool c = true;

	RTM32 tm;
//	Dbt db(100);

//	__breakpoint(0);

	InitHardware();

//	Options_Init();

	txbuf[0] = 0x5555;
	txbuf[1] = 0xAAAA;

	CopyDataDMA(txbuf, txbuf+16, 4);

	while (1)
	{
		HW::PIOA->BSET(15);

		UpdateParams();

		HW::PIOA->BCLR(15);

//		UpdateHardware();

	}; // while (1)
}
