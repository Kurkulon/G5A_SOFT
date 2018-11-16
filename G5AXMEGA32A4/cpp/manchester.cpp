#include "common.h"
#include "hardware.h"
#include "manchester.h"

static bool manchester_rx_state = false;
static byte manchester_rx_number = 0;
static byte manchester_rx_length = 0;
static u16 manchester_rx_data_temp = 0;
static u16 manchester_rx_data = 0;
static bool manchester_rx_command = false;
static bool manchester_rx_parity_temp = true;
static const bool manchester_rx_parity = true;
static bool manchester_rx_sync = false;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

manchester_rx_status_type Manchester_RX_Parse(byte length)
{	
	manchester_rx_state = !manchester_rx_state;
	if((length < (MANCHESTER_RX_BIT / 4)) || (length > ((MANCHESTER_RX_BIT * 2) + (MANCHESTER_RX_BIT / 4))))
	{
//		PORTB.OUTSET = PIN2_bm;

		manchester_rx_number = 0;
		manchester_rx_length = 0;
		manchester_rx_sync = false;

//		PORTB.OUTCLR = PIN2_bm;

		return MANCHESTER_RX_STATUS_ERROR_LENGTH;
	}
	else if(length <= ((MANCHESTER_RX_BIT) + (MANCHESTER_RX_BIT / 4)))
	{	
		manchester_rx_length ++;

		if(length > ((MANCHESTER_RX_BIT / 2) + (MANCHESTER_RX_BIT / 4))) manchester_rx_length ++;
	}
	else if(length <= ((MANCHESTER_RX_BIT * 2) + (MANCHESTER_RX_BIT / 4)))
	{
		manchester_rx_sync = true;
		manchester_rx_data_temp = 0;
		manchester_rx_parity_temp = manchester_rx_parity;
		manchester_rx_number = 0;
		manchester_rx_length = 1;

		if(length > ((MANCHESTER_RX_BIT) + (MANCHESTER_RX_BIT / 2) + (MANCHESTER_RX_BIT / 4))) manchester_rx_length = 2;

		manchester_rx_command = !manchester_rx_state;
	};

	if((manchester_rx_length == 2) && (manchester_rx_sync))
	{
  		if(manchester_rx_number < 16)
		{
			manchester_rx_data_temp <<= 1;
			manchester_rx_data_temp |= manchester_rx_state;
			manchester_rx_parity_temp ^= manchester_rx_state;
			manchester_rx_number ++;
			manchester_rx_length = 0;
		}
	 	else
		{
			manchester_rx_data = manchester_rx_data_temp;
			manchester_rx_data_temp = 0;
			manchester_rx_number = 0;
			manchester_rx_length = 0;
			manchester_rx_sync = false;

			if(manchester_rx_state != manchester_rx_parity_temp)
			{
				manchester_rx_data = (~manchester_rx_data);
				manchester_rx_state = !manchester_rx_state;
				manchester_rx_command = !manchester_rx_command;
			};

			return MANCHESTER_RX_STATUS_READY;
		}
	};

	if(manchester_rx_length >= 3)
	{
		//PORTB.OUTSET = PIN2_bm;
		//PORTB.OUTCLR = PIN2_bm;
		//PORTB.OUTSET = PIN2_bm;
		//PORTB.OUTCLR = PIN2_bm;

		manchester_rx_number = 0;
		manchester_rx_length = 0;
		manchester_rx_sync = false;


		return MANCHESTER_RX_STATUS_ERROR_STRUCT;
	};

	return MANCHESTER_RX_STATUS_HANDLE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

u16 Manchester_RX_Get_Data()
{
       	return manchester_rx_data;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool Manchester_RX_Get_Type()
{
       	return manchester_rx_command;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static byte manchester_tx_bit = 0;
static u16 manchester_tx_data = 0;
static bool manchester_tx_type = false;
static bool manchester_tx_parity_temp = true;
static const bool manchester_tx_parity = true;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Manchester_TX_Set_Data(u16 data)
{
	manchester_tx_data = data;
	manchester_tx_type = false;			
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//void Manchester_TX_Set_Type(bool type)
//{
//	manchester_tx_type = type;			
//}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
manchester_tx_status_type Manchester_TX_Parse()
{
	bool out = false;

	if(manchester_tx_bit < 3) 
	{ 
		manchester_tx_parity_temp = manchester_tx_parity;
		out = manchester_tx_type; 
	}
	else if(manchester_tx_bit < 6) 
	{
		out = !manchester_tx_type; 
	}
	else if(manchester_tx_bit < (6 + 16 * 2))
	{
		if(manchester_tx_data & 0x8000) 
		{ 
			out = !(manchester_tx_bit & 1);

			if(manchester_tx_bit & 1) 
			{ 
				manchester_tx_parity_temp ^= 1; 
				manchester_tx_data <<= 1; 
			};
		}
		else 
		{
			out = (manchester_tx_bit & 1);

			if(manchester_tx_bit & 1) 
			{ 
				manchester_tx_data <<= 1; 
			};
		};
	} 
	else if(manchester_tx_bit < (6 + 16 * 2 + 2)) 
	{
		if(manchester_tx_parity_temp) 
		{ 
			out = !(manchester_tx_bit & 1);
		}
		else 
		{
			out = (manchester_tx_bit & 1);
		};
	}
	else
	{
		manchester_tx_bit = 0;
		return MANCHESTER_TX_STATUS_READY;
	};

	manchester_tx_bit ++;

	return (!out) ? MANCHESTER_TX_STATUS_LO : MANCHESTER_TX_STATUS_HI;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

