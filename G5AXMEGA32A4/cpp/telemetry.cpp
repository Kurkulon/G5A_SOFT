#include "common.h"
#include "main.h"
#include "hardware.h"
#include "manchester.h"
#include "options.h"
#include "measure.h"
#include "control.h"
#include "telemetry.h"

volatile u16 telemetry_rx_buf[TELEMETRY_RX_BUFFER];
volatile u16 telemetry_rx_buf_count;
volatile u16 telemetry_tx_buf[TELEMETRY_TX_BUFFER];
//volatile u16 telemetry_tx_buf_copy[TELEMETRY_TX_BUFFER];
volatile u16 telemetry_tx_buf_count;
volatile u16 telemetry_tx_buf_size;
volatile telemetry_mode_type telemetry_mode = TELEMETRY_MODE_OFF;
byte telemetry_tx_baud_rate_factor = 1; 
volatile manchester_tx_status_type telemetry_tx_state = MANCHESTER_TX_STATUS_WAIT;
volatile byte telemetry_rx_timeout;

const u16 manReqWord = (u16)(TELEMETRY_ADRESS) << 8;
const u16 manReqMask = 0xFF00;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan_00(volatile u16 *data, u16 len, volatile u16 *txbuf, volatile u16 *txlen)
{
	if (len != 1) return false;

	txbuf[0] = (manReqWord & manReqMask) | 0;
	txbuf[1] = Options_Get_Serial();
	txbuf[2] = VERSION;

	*txlen = 3;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan_10(volatile u16 *data, u16 len, volatile u16 *txbuf, volatile u16 *txlen)
{
	if (len != 1) return false;

	txbuf[0] = (manReqWord & manReqMask) | 0x10;

	txbuf[1] = GetGenFreq();					//	Частота генератора(Гц), 
	txbuf[2] = GetWindowCount();				//	Количество временных окон(шт), 
	txbuf[3] = GetWindowTime();					//	Длительность временного окна(мкс), 
	txbuf[4] = Options_Get_Level_M();			//	Уровень дискриминации МЗ(у.е), 
	txbuf[5] = Options_Get_Level_B();			//	Уровень дискриминации БЗ(у.е),

	*txlen = 6;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


static bool RequestMan_20(volatile u16 *data, u16 len, volatile u16 *txbuf, volatile u16 *txlen)
{
	if (len != 1) return false;

	if (data[0] & 1)
	{
		EnableGen();
	};

	txbuf[0] = data[0];//(manReqWord & manReqMask) | 0x20;

	txbuf[1] = GetFireCount();											//2. Количество вспышек(ushort)
	txbuf[2] = GetGenWorkTime();								//3. Наработка генератора(мин)(ushort)
	txbuf[3] = Measure_Get_Current_Temperature_In();		//4. Температура в приборе(0.1 гр)(short)
	byte wc = txbuf[4] = GetWindowCount();					//5. Количество временных окон(шт)
	txbuf[5] = GetWindowTime();								//6. Длительность временного окна(мкс)

	u16 n = 6;

	for (byte i = 0; i < wc; i++)
	{
		cli();
		u32 tm = m_ts[i];
		u32 tb = b_ts[i];
		m_ts[i] = 0;
		b_ts[i] = 0;
		sei();

		txbuf[n] = (tm < 0xFFFF) ? tm : 0xFFFF;				//7..x. Спектр МЗ(ushort)
		txbuf[n+wc] = (tb < 0xFFFF) ? tb : 0xFFFF;			//x..y. Спектр БЗ(ushort)
		n += 1;
	};

	*txlen = 6 + GetWindowCount() * 2;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan_80(volatile u16 *data, u16 len, volatile u16 *txbuf, volatile u16 *txlen)
{
//	PORTB.OUTSET = PIN2_bm; PORTB.OUTCLR = PIN2_bm; 


	if (len != 3) return false;

//	PORTB.OUTSET = PIN2_bm; PORTB.OUTCLR = PIN2_bm; 

	switch (data[1])
	{
		case 1:

			Options_Set_Serial(data[2]);

			break;

		case 2:

			//for (u16 j = 0; j < (data[2]+1); j++) { PORTB.OUTSET = PIN2_bm; PORTB.OUTCLR = PIN2_bm; };

			Options_Set_Telemetry_Baud_Rate(data[2]);

			telemetry_tx_baud_rate_factor = Options_Get_Telemetry_Baud_Rate();

			break;

		default:

			return false;
	};

	txbuf[0] = (manReqWord & manReqMask) | 0x80;

	*txlen = 1;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan_90(volatile u16 *data, u16 len, volatile u16 *txbuf, volatile u16 *txlen)
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

	*txlen = 1;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan_A0(volatile u16 *data, u16 len, volatile u16 *txbuf, volatile u16 *txlen)
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

	*txlen = 1;

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan_F0(volatile u16 *data, u16 len, volatile u16 *txbuf, volatile u16 *txlen)
{
	if (len != 1) return false;

	txbuf[0] = (manReqWord & manReqMask) | 0xF0;

	*txlen = 1;

	Options_Save();

	return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static bool RequestMan(volatile u16 *buf, u16 len, volatile u16 *txbuf, volatile u16 *txlen)
{

	if (buf == 0 || len == 0 || txbuf == 0 || txlen == 0) return false;

//	PORTB.OUTSET = PIN2_bm;

	bool r = false;

	byte i = ((byte)buf[0] >> 4) & 0xF;

//	for (byte j = 0; j < (i+1); j++) { PORTB.OUTSET = PIN2_bm; PORTB.OUTCLR = PIN2_bm; };

	switch (i)
	{
		case 0: 	r = RequestMan_00(buf, len, txbuf, txlen); break;
		case 1: 	r = RequestMan_10(buf, len, txbuf, txlen); break;
		case 2: 	r = RequestMan_20(buf, len, txbuf, txlen); break;

		case 8: 	r = RequestMan_80(buf, len, txbuf, txlen); break;
		case 9:		r = RequestMan_90(buf, len, txbuf, txlen); break;
		case 0xA:	r = RequestMan_A0(buf, len, txbuf, txlen); break;
		case 0xF:	r = RequestMan_F0(buf, len, txbuf, txlen); break;
		
//		default:	bfURC++; 
	};

	return r;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

RXD_TXD_INTERRUPT()
{
	if (telemetry_mode == TELEMETRY_MODE_TX)
	{
		RXD_TXD_CLEAR_INTERRUPT();

		manchester_tx_status_type telemetry_tx_state_current = telemetry_tx_state;

		if (telemetry_tx_state_current == MANCHESTER_TX_STATUS_WAIT)
		{
			TXD_OUT_OFF();//TXD_OUT_ME();
		}
		else if (telemetry_tx_state_current == MANCHESTER_TX_STATUS_HI)
		{
			TXD_OUT_HI();
		}
		else
		{
			TXD_OUT_LO();
		};

		if (telemetry_tx_buf_count < telemetry_tx_buf_size)
		{
			telemetry_tx_state = Manchester_TX_Parse();

			if (telemetry_tx_state == MANCHESTER_TX_STATUS_READY)
			{
				telemetry_tx_buf_count++;

				if (telemetry_tx_buf_count < telemetry_tx_buf_size)
				{
					Manchester_TX_Set_Data(telemetry_tx_buf[telemetry_tx_buf_count]);
					//Manchester_TX_Set_Type(false);
					telemetry_tx_state = Manchester_TX_Parse();
				}
				else
				{
					telemetry_tx_state = MANCHESTER_TX_STATUS_WAIT;
				}
			}
		}
	}
	else if (telemetry_mode == TELEMETRY_MODE_RX)
	{
		PORTA.OUT = (PORTA.OUT & ~PIN2_bm) | ((ACA.STATUS >> 2) & PIN2_bm);

		RXD_TXD_CLEAR_INTERRUPT();

		if (Manchester_RX_Parse(RXD_Get()) == MANCHESTER_RX_STATUS_READY)
		{
			if ((Manchester_RX_Get_Type() && (telemetry_rx_buf_count == 0)) || (((!Manchester_RX_Get_Type()) && (telemetry_rx_buf_count))))
			{
				if (telemetry_rx_buf_count < TELEMETRY_RX_BUFFER)
				{
					telemetry_rx_buf[telemetry_rx_buf_count] = Manchester_RX_Get_Data();
					telemetry_rx_buf_count++;
				};
			}
			else
			{
				telemetry_rx_buf_count = 0;
			};
		};

		telemetry_rx_timeout = RXD_Timer_Get();
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
RXD_TXD_INTERRUPT()
{
//	PORTD.OUTSET = PIN0_bm;

	if(telemetry_mode == TELEMETRY_MODE_TX)
	{
		RXD_TXD_CLEAR_INTERRUPT();

		if(telemetry_tx_state == MANCHESTER_TX_STATUS_HI)
		{                                                                              
			TXD_OUT_HI();
		}
		else if(telemetry_tx_state == MANCHESTER_TX_STATUS_LO)
		{
			TXD_OUT_LO();
		}
		else if(telemetry_tx_state == MANCHESTER_TX_STATUS_READY)
		{
			TXD_OUT_ME();
			
			telemetry_tx_buf_count ++;
	              	
			if(telemetry_tx_buf_count < telemetry_tx_buf_size)
			{
				Manchester_TX_Set_Data(telemetry_tx_buf[telemetry_tx_buf_count]);
				//Manchester_TX_Set_Type(false);
			};

			telemetry_tx_state = MANCHESTER_TX_STATUS_WAIT;
		};

		if(telemetry_tx_buf_count < telemetry_tx_buf_size)
		{
			telemetry_tx_state = Manchester_TX_Parse();
		};
	}
	else if(telemetry_mode == TELEMETRY_MODE_RX)
	{
        RXD_TXD_CLEAR_INTERRUPT();

		if(Manchester_RX_Parse(RXD_Get()) == MANCHESTER_RX_STATUS_READY)		
		{	
       		if((Manchester_RX_Get_Type() && (telemetry_rx_buf_count == 0)) || (((!Manchester_RX_Get_Type()) && (telemetry_rx_buf_count))))
			{
				if(telemetry_rx_buf_count < TELEMETRY_RX_BUFFER)
				{
					telemetry_rx_buf[telemetry_rx_buf_count] = Manchester_RX_Get_Data();
					telemetry_rx_buf_count ++;
				}
			} 
			else
			{
				telemetry_rx_buf_count = 0;
			};
		}
		telemetry_rx_timeout = RXD_Timer_Get();
	}

//	PORTD.OUTCLR = PIN0_bm;
}
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//RXD_TXD_INTERRUPT()
//{
//	Telemetry_Interrupt();
//}

//inline void Telemetry_Interrupt_Check()
//{
//	if(RXD_TXD_INTERRUPT_FLAG())
//	{
//		RXD_TXD_CLEAR_INTERRUPT();
//		Telemetry_Interrupt();
//	}
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Telemetry_Init()
{
	telemetry_tx_baud_rate_factor = Options_Get_Telemetry_Baud_Rate();
	telemetry_rx_timeout = RXD_Timer_Get();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Telemetry_Idle()
{
	switch (telemetry_mode)
	{
		volatile byte t;

		case TELEMETRY_MODE_RX: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			nop();

			t = telemetry_rx_timeout; // чтобы с прерываниями не было проблем, читаем вначале
                	
			if((telemetry_rx_buf_count) && (((byte)(RXD_Timer_Get() - t)) >= (byte)((TELEMETRY_PAUSE_MKS * RXD_TIMER_FREQ)/ 1000000ul)))
			{
				RXD_Stop();

				telemetry_mode = TELEMETRY_MODE_OFF;

				if(GB(telemetry_rx_buf, 1) == TELEMETRY_ADRESS)
				{
					// PORTB.OUTSET = PIN2_bm;

	                if (RequestMan(telemetry_rx_buf, telemetry_rx_buf_count, telemetry_tx_buf, &telemetry_tx_buf_size))
					{
						//PORTB.OUTSET = PIN2_bm;

						telemetry_tx_buf_count = 0;

						Manchester_TX_Set_Data(telemetry_tx_buf[0]);
						//Manchester_TX_Set_Type(false);

						telemetry_mode = TELEMETRY_MODE_TX;

						TXD_Set((TXD_TIMER_FREQ / (TELEMETRY_BAUD_RATE * telemetry_tx_baud_rate_factor)));
						TXD_Start();
					}
					else
					{
						telemetry_mode = TELEMETRY_MODE_OFF;
					};

					telemetry_rx_buf_count = 0;	
				};

				//PORTB.OUTCLR = PIN2_bm;
			};

			break;

		case TELEMETRY_MODE_TX: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			{
				cli();
				u16 temp = telemetry_tx_buf_count;
				sei();
	                	
				if(temp >= telemetry_tx_buf_size)
				{
					telemetry_mode = TELEMETRY_MODE_OFF;
					telemetry_tx_buf_count = 0;		
					telemetry_tx_buf_size = 0;
					TXD_Stop();
				};
			};

			break;

		case TELEMETRY_MODE_OFF: //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			telemetry_mode = TELEMETRY_MODE_RX;
			telemetry_rx_buf_count = 0;

			RXD_Start();

			break;
	}

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
